// CopyRight kang


#include "AbilitySystem/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::  CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

// 激活该 Ability Task（通常在 GameplayAbility 的 ActivateAbility 中调用）
void UTargetDataUnderMouse::Activate()
{
	// 判断当前技能的拥有者（通常是 PlayerController）是否由本地客户端控制
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	if (bIsLocallyControlled)
	{
		// 如果是本地客户端控制，则主动拾取鼠标下的目标，并发送给服务器
		SendMouseCursorData();
	}
	else
	{
		// 如果是服务器端（或模拟代理），则等待客户端发送目标数据
		// 获取当前技能的唯一标识：SpecHandle 和 PredictionKey（用于网络预测匹配）
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		// 注册一个委托：当服务器收到客户端复制过来的目标数据时，调用 OnTargetDataReplicatedCallback
		// AbilityTargetDataSetDelegate 是一个多播委托，用于监听目标数据的到达
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
			.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		// 尝试立即调用已缓存的目标数据委托（例如：服务器在之前已收到数据）
		// CallReplicatedTargetDataDelegatesIfSet 会检查是否有已接收的数据并触发委托
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(
			SpecHandle, ActivationPredictionKey);

		if (!bCalledDelegate)
		{
			// 如果尚未收到数据，说明需要等待客户端上传
			// 设置任务状态为“等待远程玩家数据”，防止任务提前结束
			SetWaitingOnRemotePlayerData();
		}
	}
}

// 由本地客户端调用：拾取鼠标下的目标并发送给服务器
void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 创建一个“预测窗口”（Scoped Prediction Window）// 第二个参数 true 表示这是一个“客户端发起的预测操作”
	// 它会自动处理预测键（Prediction Key）的生成和同步，确保客户端和服务器操作对齐
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), true);

	// 获取当前玩家控制器（用于拾取）
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;

	// 执行鼠标拾取：从摄像机发射一条射线，检测可见物体（ECC_Visibility 通道）
	// false 表示不追踪复杂碰撞（只用简单碰撞体）
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

	// 创建目标数据对象：单目标命中（SingleTargetHit）
	// 这是 GAS 内置的一种 TargetData 类型，封装了 HitResult
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;

	// 将数据包装进 TargetDataHandle（GAS 要求用 Handle 传递，支持多目标）
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);

	// 调用服务器 RPC：将目标数据复制到服务器
	// 参数说明：
	//   - SpecHandle: 技能实例标识
	//   - PredictionKey: 预测键，用于匹配客户端预测与服务器执行
	//   - DataHandle: 实际目标数据
	//   - FGameplayTag(): 激活标签（此处未使用，可为空）
	//   - ScopedPredictionKey: 当前预测窗口的键（用于验证）
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	// 检查是否应该广播“有效数据”事件（通常在客户端预测成功时触发）
	// ShouldBroadcastAbilityTaskDelegates() 确保不会在无效状态（如技能已结束）下广播
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// 广播 ValidData 事件，通知技能逻辑可以使用此目标数据（例如施放技能）
		ValidData.Broadcast(DataHandle);
	}
}

// 服务器收到客户端目标数据后，或客户端收到服务器确认后，调用此回调
void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& DataHandle, 
	FGameplayTag ActivationTag)
{
	// 告诉 AbilitySystemComponent：该预测键对应的目标数据已被“消费”（处理完毕）
	// 这会清除内部缓存，避免重复处理
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetAbilitySpecHandle(), 
		GetActivationPredictionKey());

	// 同样检查是否应广播有效数据
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		// 广播目标数据，供 GameplayAbility 使用（例如调用 CommitExecute）
		ValidData.Broadcast(DataHandle);
	}
}



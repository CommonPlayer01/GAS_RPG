// CopyRight kang


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	//判断当前是否拾取到内容
	if(HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		//取消技能
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if(CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

/**
 * @brief 执行一次球形射线检测（Sphere Trace），从武器尖端向目标位置发射，
 *        用于确定光束技能的第一个命中点和命中对象。
 *
 * 此函数通常在鼠标点击或技能目标位置确定后调用，用于获取实际的物理命中结果，
 * 以便后续生成光束特效、造成伤害等。
 *
 * @param BeamTargetLocation 鼠标或输入指定的目标位置（通常是屏幕投影到世界的位置）
 */
void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
    // 确保拥有者角色（OwnerCharacter）有效，并且实现了 UCombatInterface 接口
    // 这是为了安全地调用接口函数（如 GetWeapon）
    if (OwnerCharacter && OwnerCharacter->Implements<UCombatInterface>())
    {
        // 通过战斗接口获取角色当前持有的武器 SkeletalMeshComponent
        // Execute_GetWeapon 是 UE 自动生成的接口调用函数
        if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
        {
            // 创建一个需要忽略的 Actor 列表，避免射线击中自己或相关对象
            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(OwnerCharacter); // 忽略施法者自身，防止自伤或无效命中

            // 从武器模型的 "TipSocket" 插槽获取发射起点
            // TipSocket 应在武器 Skeletal Mesh 中预先定义，通常位于枪口/法杖尖端
            const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

            // 声明命中结果结构体，用于接收 Trace 的输出信息
            FHitResult HitResult;

            // 执行单次球形射线检测（Sphere Trace）
            // 相比 LineTrace，SphereTrace 使用一个小球体进行碰撞检测，更不容易“穿模”
            UKismetSystemLibrary::SphereTraceSingle(
                OwnerCharacter,                 // WorldContextObject：用于获取世界，通常传入 Actor
                SocketLocation,                 // 起始位置：武器尖端
                BeamTargetLocation,             // 结束位置：目标点（如鼠标位置）
                10.f,                           // 球体半径（单位：厘米），值越大越不容易漏检
                TraceTypeQuery1,                // 碰撞通道（Collision Channel），通常设为 ECC_Visibility 或自定义通道
                false,                          // 是否返回物理材质信息（一般不需要）
                ActorsToIgnore,                 // 要忽略的 Actor 列表（避免击中自己）
                EDrawDebugTrace::None,          // 是否绘制调试线（None = 不绘制，ForDuration = 临时绘制）
                HitResult,                      // 输出：命中结果
                true                            // 是否命中的阻挡物才记录（true = 只记录 Blocking Hit）
            );

            // 检查是否成功命中了阻挡物（bBlockingHit 表示发生了有效碰撞）
            if (HitResult.bBlockingHit)
            {
                // 保存命中点的世界坐标（用于生成光束终点、特效、伤害位置等）
                MouseHitLocation = HitResult.ImpactPoint;

                // 保存被命中的 Actor（用于后续造成伤害、应用效果等）
                MouseHitActor = HitResult.GetActor();
            }
            else
            {
                // 可选：若未命中任何物体，可将目标设为 BeamTargetLocation（例如打到空中）
                // MouseHitLocation = BeamTargetLocation;
                // MouseHitActor = nullptr;
            }
        }
    }
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::PrimaryTargetDied);
		}
	}
}



void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	
	TArray<AActor*> ActorsToIgnore; //遍历时忽略的数组
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo()); //忽略自身
	ActorsToIgnore.Add(MouseHitActor); //忽略鼠标命中的的敌人

	TArray<AActor*> OverlappingActors; //存放遍历结果的数组

	//通过封装的函数获取到技能范围内散射的目标
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation());

	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	// int32 NumAdditionalTargets = 5;

	//通过自定义函数来获取最近的几个目标
	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}





// CopyRight kang


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"


UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag)
{
	UWaitCooldownChange* WaitCooldownChange = NewObject<UWaitCooldownChange>();
	WaitCooldownChange->ASC = AbilitySystemComponent;
	WaitCooldownChange->CooldownTag = InCooldownTag;

	//如果参数有一项未设置，我们将结束此任务
	if(!IsValid(AbilitySystemComponent) || !InCooldownTag.IsValid())
	{
		WaitCooldownChange->EndTask();
		return nullptr;
	}

	//监听冷却标签的变动，并绑定回调，用于获取冷却结束
	AbilitySystemComponent->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved)
	.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnCooldownTagChanged);

	//监听GE应用回调，获取冷却激活，用于获取技能开始进入冷却
	AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCooldownChange, &UWaitCooldownChange::OnActiveEffectAdded);

	return WaitCooldownChange;
}

void UWaitCooldownChange::EndTask()
{
	//判断ASC是否存在
	if(!IsValid(ASC)) return;
	
	//取消对冷却标签的变动监听
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	SetReadyToDestroy(); //设置此对象可以被销毁，如果此对象不再被引用，将可以被销毁掉
	MarkAsGarbage(); //标记此实例为垃圾资源，可以被回收
}

void UWaitCooldownChange::OnCooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount) const
{
	//如果计数为0，代表冷却标签已经不存在，技能不处于冷却状态
	if(NewCount == 0)
	{
		//广播冷却结束委托
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle) const
{
	// --- 1. 标签提取 ---
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags); // 获取 GE 自身的静态标签（如标识该 GE 为 Cooldown 类型）

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags); // 获取该 GE 赋予目标 Actor 的动态标签

	// --- 2. 匹配验证 ---
	// 检查当前应用的 GE 是否关联了我们正在监听的特定 CooldownTag
	if(AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		// --- 3. 构建查询条件 ---
		// 创建查询器，目标是寻找所有拥有该冷却标签的激活中效果（Active Gameplay Effects）
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
       
		// --- 4. 检索剩余时间 ---
		// 获取所有匹配查询条件的 GE 的剩余时间列表
		TArray<float> TimesRemaining = TargetASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
       
		if(TimesRemaining.Num() > 0)
		{
			// --- 5. 寻找极值（最大冷却） ---
			// 在所有并行的冷却效果中，提取出最长的那一个作为当前的真实冷却进度
			float TimeRemaining = TimesRemaining[0];
			for(int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if(TimeRemaining < TimesRemaining[i]) TimeRemaining = TimesRemaining[i];
			}
          
			// --- 6. 结果分发 ---
			// 将计算出的初始冷却时长通过委托（Delegate）发送出去，通常用于更新 UI 进度条
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}

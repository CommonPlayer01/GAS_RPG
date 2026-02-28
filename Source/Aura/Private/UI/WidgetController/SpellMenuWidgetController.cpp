// CopyRight kang


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	//绑定技能状态修改后的委托回调
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Newlevel)
	{
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.StatusTag = StatusTag;
			BroadcastSpellGlobeSelected();
		}
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag); //获取到技能数据
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	//监听技能装配的回调
	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	//绑定技能点变动回调
	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 SpellPoints)
	{
		SpellPointChangedDelegate.Broadcast(SpellPoints); //广播拥有的技能点
		
		CurrentSpellPoints = SpellPoints;

		BroadcastSpellGlobeSelected(); //广播升降级按钮状态
	});

}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

FGameplayTag USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType;
		StopWaitForEquipSelectionDelegate.Broadcast(SelectedAbilityType);
		bWaitForEquipSelection = false;
	}

	
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints(); //获取技能点数
	FGameplayTag AbilityStatus;

	const bool bTagValid = AbilityTag.IsValid(); //判断传入的标签是否存在
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None); //判断传入的是否为空技能标签
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag); //通过技能标签获取技能
	const bool bSpecValid = AbilitySpec != nullptr; //判断技能实例是否存在
	
	if(!bTagValid || bTagNone || !bSpecValid)
	{
		//传入标签不存在，或传入的为空技能标签，或者技能实例不存在时，设置为技能按钮显示上锁状态
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		//从技能实例获取技能的状态标签
		AbilityStatus = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	//选中时，更新控制器缓存数据
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = AbilityStatus;
	CurrentSpellPoints = SpellPoints;

	BroadcastSpellGlobeSelected(); //广播升降级按钮状态
	

	return AbilityStatus;
}

void USpellMenuWidgetController::BroadcastSpellGlobeSelected()
{
	bool bEnableSpendPoints = false; //技能是否可以升级
	bool bEnableEquip = false; //技能是否可以装配
	bool bEnableDemotion = false; //技能是否可以降级

	ShouldEnableButtons(SelectedAbility.StatusTag, CurrentSpellPoints > 0, bEnableSpendPoints, bEnableEquip, bEnableDemotion); //获取结果

	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip, bEnableDemotion); //广播状态

	//广播技能描述
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
	SpellDescriptionDelegate.Broadcast(Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if(GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag); //调用ASC等级提升函数
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	StopWaitForEquipSelectionDelegate.Broadcast(SelectedAbilityType);
	bWaitForEquipSelection = false;

	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	SelectedAbility.AbilityTag = GameplayTags.Abilities_None;
	SelectedAbility.StatusTag = GameplayTags.Abilities_Status_Locked;
	// SelectedAbility.Level = 0;

	SpellDescriptionDelegate.Broadcast(FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	// const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	//获取装配技能的类型
	const FGameplayTag& SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	WaitForEquipSelectionDelegate.Broadcast(SelectedAbilityType);
	bWaitForEquipSelection = true;

	const FGameplayTag SelectedStatus = GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot = GetAuraASC()->GetSlotTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
	
	// if(!SelectedAbilityType.MatchesTagExact(AbilityType)) return; //类型不同无法装配

// 	//获取装配技能的输入标签
// 	if(SelectedAbilityInputTag.MatchesTagExact(SlotTag)) return; //如果当前技能输入和插槽标签相同，证明已经装配，不需要再处理
//
// 	//调用装配技能函数，进行处理
// 	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability, SlotTag);
}

void USpellMenuWidgetController::SpellRowGlobeSelected(const FGameplayTag& SlotnputTag, const FGameplayTag& AbilityType)
{
	if (!bWaitForEquipSelection) return;
	const FGameplayTag&	SelectedAbilityType = AbilityInfo->FindAbilityInfoForTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, SlotnputTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitForEquipSelection = false;
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	//清除旧插槽的数据
	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreviousSlot;
	LastSlotInfo.AbilityTag = GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	//更新新插槽的数据
	FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag = Status;
	Info.InputTag = Slot;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitForEquipSelectionDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);
	SpellGlobeReassignedDelegate.Broadcast(AbilityTag);
	GlobeDeselect();
}


/*
 * 根据状态和是否拥有可分配的技能点数，来设置对应的状态
 */
void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool HasSpellPoints,
	bool& bShouldEnableSpellPoints, bool& bShouldEnableEquip, bool& bShouldDemotionPoints)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableSpellPoints = HasSpellPoints;
		bShouldEnableEquip = true;
		bShouldDemotionPoints = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		bShouldEnableSpellPoints = HasSpellPoints;
		bShouldEnableEquip = false;
		bShouldDemotionPoints = false;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableSpellPoints = HasSpellPoints;
		bShouldEnableEquip = true;
		bShouldDemotionPoints = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
		bShouldEnableSpellPoints = false;
		bShouldEnableEquip = false;
		bShouldDemotionPoints = false;
	}
}

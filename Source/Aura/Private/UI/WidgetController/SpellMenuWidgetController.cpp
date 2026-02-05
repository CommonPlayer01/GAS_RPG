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

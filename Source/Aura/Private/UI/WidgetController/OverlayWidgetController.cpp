// CopyRight kang


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();

	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());

	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());

	BroadcastAbilityInfo();
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	//绑定等级相关回调
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &ThisClass::OnXPChanged);
	//绑定等级相关回调
	GetAuraPS()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangeDelegate.Broadcast(NewLevel);
	});

	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		GetAuraAS()->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

	if(GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if(GetAuraASC()->bStartupAbilitiesGiven)
		{
			//如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			BroadcastInitialValues();
		}
		else
		{
			//如果执行到此处，技能初始化还未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			GetAuraASC()->AbilityGivenDelegate.AddUObject(this, &ThisClass::BroadcastInitialValues);
		}

		//AddLambda 绑定匿名函数
		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags) //中括号添加this是为了保证内部能够获取类的对象
			{
				for(const FGameplayTag& Tag : AssetTags)
				{

					//对标签进行检测，如果不是信息标签，将无法进行广播
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					// "A.1".MatchesTag("A") will return True, "A".MatchesTag("A.1") will return False
					if(Tag.MatchesTag(MessageTag))
					{
						FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row); //前面加*取消指针引用
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("无法查询到等级相关数据，请查看PlayerState是否设置了对应的数据"));

	const int32 Level =  LevelUpInfo->FindLevelForXP(NewXP); //获取当前等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num(); //获取当前最大等级

	if(Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement; //当前等级升级所需经验值
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement; //上一级升级所需经验值

		const float XPPercent = static_cast<float>((NewXP - PreviousLevelUpRequirement) * 1.0 / (LevelUpRequirement - PreviousLevelUpRequirement)); //计算经验百分比
		OnXPPercentChangedDelegate.Broadcast(XPPercent); //广播经验条比例
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
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

}

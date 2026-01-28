// CopyRight kang


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	const UAuraAttributeSet* AttributeSetBase = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AttributeSetBase->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSetBase->GetMaxHealth());

	OnManaChanged.Broadcast(AttributeSetBase->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSetBase->GetMaxMana());

}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
	const UAuraAttributeSet* AttributeSetBase = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetHealthAttribute()).AddUObject(this, &UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetMaxHealthAttribute()).AddUObject(this, &UOverlayWidgetController::MaxHealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetManaAttribute()).AddUObject(this, &UOverlayWidgetController::ManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetMaxManaAttribute()).AddUObject(this, &UOverlayWidgetController::MaxManaChanged);

	if(UAuraAbilitySystemComponent* AuraSAC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		if(AuraSAC->bStartupAbilitiesGiven)
		{
			//如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			OnInitializeStartupAbilities(AuraSAC);
		}
		else
		{
			//如果执行到此处，技能初始化还未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			AuraSAC->AbilityGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
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

void UOverlayWidgetController::OnInitializeStartupAbilities(UAuraAbilitySystemComponent* AuraAbilitySystemComponent) const
{
	if(!AuraAbilitySystemComponent->bStartupAbilitiesGiven) return; //判断当前技能初始化是否完成，触发回调时都已经完成
	
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		//通过静态函数获取到技能实例的技能标签，并通过标签获取到技能数据
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec));
		//获取到技能的输入标签
		FGameplayTag Tag =  UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		Info.InputTag = Tag;
		//广播技能数据
		AbilityInfoDelegate.Broadcast(Info); 
	});
	//遍历技能并触发委托回调
	AuraAbilitySystemComponent->ForEachAbility(BroadcastDelegate);

}

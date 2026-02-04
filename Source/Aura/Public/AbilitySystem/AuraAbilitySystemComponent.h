// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"
 
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE(FAbilityGiven) //技能初始化应用后的回调委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&); //单播委托，只能绑定一个回调
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/);


/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;
	FAbilityGiven AbilityGivenDelegate; //技能初始化应用后的回调委托
	FAbilityStatusChanged AbilityStatusChanged; //技能状态更新委托


	bool bStartupAbilitiesGiven = false; //初始化应用技能后，此值将被设置为true，用于记录当前是否被初始化完成
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities); //应用被动技能


	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHold(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	void ForEachAbility(const FForEachAbility& Delegate); //遍历技能，并将技能广播出去

	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag); //通过技能标签获取技能实例

	void UpdateAbilityStatuses(int32 Level); //根据角色等级更新技能数据状态

	virtual void OnRep_ActivateAbilities() override;

	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag); //服务器升级属性函数


protected:
	//标记Client告诉UE这个函数应该只在客户端运行，设置Reliable表示这个函数调用是可靠的，即它确保数据能够到达客户端
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag); //技能状态更新后回调

};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"
 
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* AssetTags */)
DECLARE_MULTICAST_DELEGATE(FAbilityGiven) //技能初始化应用后的回调委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&); //单播委托，只能绑定一个回调
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*技能标签*/, const FGameplayTag& /*技能状态标签*/, const FGameplayTag& /*输入标签*/, const FGameplayTag& /*上一个输入标签*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag& /*技能标签*/); //中止一个技能激活的回调
DECLARE_MULTICAST_DELEGATE_TwoParams(FActivePassiveEffect, const FGameplayTag& /*被动技能标签*/, bool /*激活或取消*/); //被动技能特效监听委托，对应特效是否开启


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
	FAbilityEquipped AbilityEquipped; //技能装配更新回调
	FDeactivatePassiveAbility DeactivatePassiveAbility; //取消技能激活的委托
	FActivePassiveEffect ActivatePassiveEffect; //被动技能对应特效委托

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
	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetSlotTagFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	bool AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag);
	bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;
	void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	// void ClearSlot(FGameplayAbilitySpec* Spec);
	// void ClearAbilitiesOfSlot(const FGameplayTag& Slot);

		
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag); //通过技能标签获取技能实例

	void UpdateAbilityStatuses(int32 Level); //根据角色等级更新技能数据状态

	virtual void OnRep_ActivateAbilities() override;

	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag); //服务器升级属性函数
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag); //只在服务器端运行，消耗技能点函数提升技能等级

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& SelectedAbilityTag, const FGameplayTag& SlotInputTag);

	UFUNCTION(Client, Reliable) //在客户端处理技能装配
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	/**
	 * 多网络被动特效委托广播，让每个客户端都可以看到特效
	 * @param AbilityTag 被动技能标签
	 * @param bActivate 激活或者关闭
	 */
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	

	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription); //通过标签获取技能描述

	void ClearSlot(FGameplayAbilitySpec* Spec); //清除技能装配插槽的技能
	
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot); //根据输入标签，清除技能装配插槽的技能

    static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot); //判断当前技能实例是否处于目标技能装配插槽


protected:
	//标记Client告诉UE这个函数应该只在客户端运行，设置Reliable表示这个函数调用是可靠的，即它确保数据能够到达客户端
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel); //技能状态更新后回调


};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSpellGlobeSelectedSignature, bool, bSpendPointsEnabled, bool, bEquipEnabled, bool, bDemotionPointsEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellDescriptionSignature, FString, SpellDescription, FString, SpellNextLevelDescription); //技能描述委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature, const FGameplayTag&, AbilityType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpellGlobeReassignedSignature, const FGameplayTag&, AbilityTag);


//在技能面板选中的技能的标签结构体
struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag(); //技能标签
	FGameplayTag StatusTag = FGameplayTag(); //技能状态标签
	int32 Level = 0;
};


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
public:

	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature SpellPointChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate; //选中技能按钮后，升级和装备按钮的变动回调

	UPROPERTY(BlueprintAssignable)
	FSpellDescriptionSignature SpellDescriptionDelegate; //选中技能按钮后，技能和下一级描述委托
	
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitForEquipSelectionDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeReassignedDelegate;
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag SpellGlobeSelected(const FGameplayTag& AbilityTag); //技能按钮选中调用函数，处理升级按钮和装配

	//广播当前技能按钮升降级按钮状态和可装配状态
	void BroadcastSpellGlobeSelected();

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed(); //升级按钮调用函数

	UFUNCTION(BlueprintCallable)
	void GlobeDeselect(); //取消按钮选中处理
	
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType); //装配技能按钮按下事件

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobeSelected(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType);

	//监听技能装配后的处理
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);


private:

	//通过技能状态标签和可分配技能点数来获取技能是否可以装配和技能是否可以升级
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool HasSpellPoints, bool& bShouldEnableSpellPoints, bool& bShouldEnableEquip, bool& bShouldDemotionPoints);
	//设置选中技能默认值
	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	//保存当前技能可分配点数
	int32 CurrentSpellPoints = 0;

	bool bWaitForEquipSelection = false;

	FGameplayTag SelectedSlot;
};

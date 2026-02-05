// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSpellGlobeSelectedSignature, bool, bSpendPointsEnabled, bool, bEquipEnabled, bool, bDemotionPointsEnabled);

//在技能面板选中的技能的标签结构体
struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag(); //技能标签
	FGameplayTag StatusTag = FGameplayTag(); //技能状态标签
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
	FOnPlayerStateChangedSignature SpellPointChanged;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedSignature; //选中技能按钮后，升级和装备按钮的变动回调
	
	UFUNCTION(BlueprintCallable)
	FGameplayTag SpellGlobeSelected(const FGameplayTag& AbilityTag); //技能按钮选中调用函数，处理升级按钮和装配

	//广播当前技能按钮升降级按钮状态和可装配状态
	void BroadcastSpellGlobeSelected() const;

	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed(); //升级按钮调用函数
private:

	//通过技能状态标签和可分配技能点数来获取技能是否可以装配和技能是否可以升级
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatus, bool HasSpellPoints, bool& bShouldEnableSpellPoints, bool& bShouldEnableEquip, bool& bShouldDemotionPoints);
	//设置选中技能默认值
	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	//保存当前技能可分配点数
	int32 CurrentSpellPoints = 0;

};

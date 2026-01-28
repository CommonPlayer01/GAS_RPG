// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"

	struct FGameplayAbilitySpec;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);
/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart; //冷却触发开始委托

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd; //冷却结束委托

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCooldownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag; //记录监听的冷却标签

	//监听冷却标签回调函数
	void OnCooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount) const;

	//监听ASC激活GE的回调
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle) const;
};

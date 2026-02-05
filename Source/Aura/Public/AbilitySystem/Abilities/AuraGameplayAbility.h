// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	UPROPERTY(EditDefaultsOnly, Category="Test")
	FGameplayTag TestGameAbilityStatus;


	virtual FString GetDescription(int32 Level); //获取当前等级的技能描述
	virtual FString GetNextLevelDescription(int32 Level); //获取技能下一等级的技能描述
	static  FString GetLockedDescription(int32 Level); //获取锁定技能描述


	
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:

	float GetManaCost(float InLevel = 1.f) const; //获取技能蓝量消耗
	float GetCooldown(float InLevel = 1.f) const; //获取技能冷却时间

};

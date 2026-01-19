// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/*
	* struct FScalableFloat
	{
		float Value;                      // 基础值（默认值）
		TMap<FName, float> ScalabilityMap; // 按可扩展性级别（如 "Low", "Medium", "High"）映射的值
	};
	 */

	UPROPERTY(EditDefaultsOnly,Category="Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "ArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UArcaneShards : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述
	FString GetDescriptionAtLevel(int32 Level, const FString& Title); //获取对应等级的技能描述

	UPROPERTY(BlueprintReadOnly)
	int32 MaxNumShards = 11;
};




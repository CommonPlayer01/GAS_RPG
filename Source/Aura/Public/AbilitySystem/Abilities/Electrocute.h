// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述
	FString GetDescriptionAtLevel(int32 Level, const FString& Title); //获取对应等级的技能描述
};

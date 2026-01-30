// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP) const; //根据经验获取等级

	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const; //获取当前经验值

	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointsReward(int32 Level) const; //获取属性点奖励

	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointsReward(int32 Level) const; //获取技能点奖励

	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXP); //增加经验

	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel); //增加等级

	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoints(int32 InAttributePoints); //增加属性点

	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoints(int32 InSpellPoints); //增加技能点

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp(); //升级

};

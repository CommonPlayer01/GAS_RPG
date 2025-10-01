// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override; //服务器初始化ASC
	virtual void OnRep_PlayerState() override; //客户端初始化ASC

	/* ICombatInterface战斗接口 */
	virtual int32 GetPlayerLevel() override;
	/* ICombatInterface战斗接口 结束 */

	
private:
	virtual void InitAbilityActorInfo() override;
};

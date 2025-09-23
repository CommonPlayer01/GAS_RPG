// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	virtual void HighlightActor() override; //高亮
	virtual void UnHighlightActor() override; //取消高亮

	UPROPERTY(BlueprintReadOnly) //蓝图可读
	bool bHighlighted = false; //是否高亮
protected:
	virtual void BeginPlay() override;
	
};

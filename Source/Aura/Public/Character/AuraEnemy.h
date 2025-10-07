// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();
	/* IEnemyInterface敌人接口 */
	virtual void HighlightActor() override; //高亮
	virtual void UnHighlightActor() override; //取消高亮
	/* IEnemyInterface敌人接口 */

	/* ICombatInterface战斗接口 */
	virtual int32 GetPlayerLevel() override;
	/* ICombatInterface战斗接口 结束 */


	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	
	UPROPERTY(BlueprintReadOnly) //蓝图可读
	bool bHighlighted = false; //是否高亮
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HealthBar;

};

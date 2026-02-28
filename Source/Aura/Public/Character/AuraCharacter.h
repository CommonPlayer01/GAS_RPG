// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override; //服务器初始化ASC
	virtual void OnRep_PlayerState() override; //客户端初始化ASC

	/* Player Interface */
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointsReward_Implementation(int32 Level) const override;
	virtual int32 GetSpellPointsReward_Implementation(int32 Level) const override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual int32 GetAttributePoints_Implementation() const override; 
	virtual void AddToAttributePoints_Implementation(int32 InAttributePoints) override;
	virtual int32 GetSpellPoints_Implementation() const override;
	virtual void AddToSpellPoints_Implementation(int32 InSpellPoints) override;
	virtual void ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial) override;
	virtual void HideMagicCircle_Implementation() override;

	/* Player Interface*/

	
	/* ICombatInterface战斗接口 */
		
	virtual int32 GetPlayerLevel_Implementation() override;
	/* ICombatInterface战斗接口 结束 */

	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent; //升级特效组件

	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	
private:
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent; //相机组件

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom; //弹簧臂组件

	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastLevelUpParticles() const; //在多人游戏，每个客户端上播放升级特效

};

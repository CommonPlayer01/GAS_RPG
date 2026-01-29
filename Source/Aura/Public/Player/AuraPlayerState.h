// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32);


/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	AAuraPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; } //获取as

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo; //设置升级相关数据

	FOnPlayerStateChanged OnXPChangedDelegate; //经验值变动委托
	FOnPlayerStateChanged OnLevelChangedDelegate; //等级变动委托

	int32 GetPlayerLevel() const{return Level;}

	void AddToLevel(int32 InLevel); //增加等级
	void SetLevel(int32 InLevel); //设置当前等级

	FORCEINLINE int32 GetXP() const {return XP;} //获取角色当前经验值
	void AddToXP(int32 InXP); //增加经验值
	void SetXP(int32 InXP); //设置当前经验值


protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0.f;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const; //服务器出现更改自动同步到本地函数 等级

	UFUNCTION()
	void OnRep_XP(int32 OldXP) const; //服务器出现更改自动同步到本地函数 经验值

};


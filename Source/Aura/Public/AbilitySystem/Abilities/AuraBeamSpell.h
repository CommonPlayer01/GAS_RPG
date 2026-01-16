// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraBeamSpell : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
public:
	/**
	 * 将鼠标拾取命中信息存储
	 * @param HitResult 在技能中通过TargetDataUnderMouse的task获取到的结果
	 */
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);

	/**
	 * 设置拥有当前技能的玩家控制器
	 */
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();

	/**
	 * 拾取闪电链命中的第一个目标
	 * @param BeamTargetLocation 鼠标点击目标位置
	 * @note 有可能中间会被阻挡，拾取的目标不是鼠标选中的目标
	 */
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);

	
	/**
	 * 通过技能命中目标获取扩散的敌人目标
	 * @param OutAdditionalTargets 返回获取到的最近的目标数组
	 */
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);


protected:

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	FVector MouseHitLocation; //鼠标拾取位置

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<AActor> MouseHitActor; //鼠标拾取的对象

	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<APlayerController> OwnerPlayerController; //拥有当前技能的玩家控制器
	
	UPROPERTY(BlueprintReadWrite, Category="Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 MaxNumShockTargets = 5; //最大散射的闪电链数

};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"


class USplineComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override; //游戏开始时触发
	virtual void SetupInputComponent() override; //在生成输入组件时触发
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> CharacterContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;
	
	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace(); //鼠标位置追踪拾取
	FHitResult CursorHit;
	IEnemyInterface* LastActor; //上一帧拾取到的接口指针
	IEnemyInterface* ThisActor; //这一帧拾取到的接口指针


	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void AbilityInputTagHold(const FGameplayTag InputTag);


	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	
	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector; //存储鼠标点击的位置
	float FollowTime = 0.f; // 用于查看按住了多久
	bool bAutoRunning = false; //当前是否自动移动
	bool bTargeting = false; //当前鼠标是否选中敌人

	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.3f; //定义鼠标悬停多长时间内算点击事件

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f; //当角色和目标距离在此半径内时，将关闭自动寻路

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline; //自动寻路时生成的样条线

	void AutoRun();
	

};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

//效果应用状态枚举
UENUM(BlueprintType) 
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

//效果移除的状态枚举
UENUM(BlueprintType) 
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove
};



UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraEffectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable) 
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);//给与目标添加GameplayEffect

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	bool bDestroyOnEffectRemoval = false;

	//在重叠开始时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable) 
	void OnOverlap(AActor* TargetActor);

	//在重叠结束时处理效果的添加删除逻辑
	UFUNCTION(BlueprintCallable) 
	void OnEndOverlap(AActor* TargetActor);

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass; //生成GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass; //生成具有一定持续时间的GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	TSubclassOf<UGameplayEffect> InfinityGameplayEffectClass; //生成具有一定持续时间的GameplayEffect的类

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectApplicationPolicy InfinityEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Apply Effects")
	EEffectRemovalPolicy InfinityEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;

	//用于存储当前已经激活的GameplayEffect的句柄的map
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};

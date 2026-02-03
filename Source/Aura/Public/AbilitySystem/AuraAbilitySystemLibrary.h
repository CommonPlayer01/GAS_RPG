// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UAbilityInfo;
struct FWidgetControllerParams;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;
class USpellMenuWidgetController;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 *控制器相关
	 ***/
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWcParams, AAuraHUD*& OutRPGHUD);

	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);
	//获取属性面板的控制器
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|WidgetController", meta=(DefaultToSelf = "WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	/**
	 *控制器相关
	 ***/

	//初始化角色的属性
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	//初始化角色的技能
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);
	
	//获取角色配置数据
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	//获取玩家角色技能配置数据
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|CharacterClassDefaults", meta=(DefaultToSelf = "WorldContextObject"))
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	//获取当前GE是否触发格挡
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	//获取当前GE是否触发暴击
	UFUNCTION(BlueprintPure, Category="MyAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	
	UFUNCTION(BlueprintCallable, Category="MyAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);
	/**
	 * 获取距离目标位置最近的几个目标
	 * @param MaxTargets 获取最大目标的数量
	 * @param Actors 需要计算的目标数组
	 * @param OutClosestTargets 返回获取到的最近的目标
	 * @param Origin 计算的位置
	 */
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|GameplayMechanics")
	static void GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors, TArray<AActor*>& OutClosestTargets, const FVector& Origin);

	//获取双方是否为敌对势力
	UFUNCTION(BlueprintPure, Category="RPGAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	//获取根据敌人类型和等级获取敌人产生的经验
	UFUNCTION(BlueprintCallable, Category="RPGAbilitySystemLibrary|CharacterClassDefaults")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

};

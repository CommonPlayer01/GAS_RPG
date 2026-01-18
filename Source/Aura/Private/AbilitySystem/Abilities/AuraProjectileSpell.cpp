	// CopyRight kang


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}


void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
	
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		// 获取武器或角色身上用于发射投射物的战斗插槽（Socket）的世界位置
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo());
		// 计算从插槽位置指向目标位置的方向向量，并将其转换为旋转（FRotator）
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		// 强制将俯仰角（Pitch）设为 0，使发射方向始终保持水平（平行于地面）
		Rotation.Pitch = 0.f;
		
		FTransform SpawnTransform;
		// 设置投射物的生成位置为战斗插槽的位置
		SpawnTransform.SetLocation(CombatInterface->GetCombatSocketLocation());
		// 将调整后的旋转（无俯仰）转换为四元数，并设置为投射物的朝向
		SpawnTransform.SetRotation(Rotation.Quaternion());

		
		//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		//TODO:设置一个用于造成伤害的游戏效果

		// 获取当前技能所属角色（Avatar Actor）的 AbilitySystemComponent（即伤害来源的 ASC）
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this); //设置技能
		EffectContextHandle.AddSourceObject(Projectile); //设置GE的源
		//添加Actor列表
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		//添加命中结果
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
		//添加技能触发位置
		EffectContextHandle.AddOrigin(ProjectileTargetLocation);

		
		// 基于伤害效果类（DamageEffectClass）、当前技能等级和上下文，创建一个传出的 GameplayEffect 规格（Spec）
		// 此 Spec 将用于后续对目标应用伤害
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// 获取全局 GameplayTags 单例，用于访问预定义的标签
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();

		for (auto Pair : DamageTags)
		{
			const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
		}

		// 将配置好的伤害效果句柄赋值给投射物，以便命中时使用
		Projectile->DamageEffectHandle = SpecHandle;

		// 调用 FinishSpawning 确保投射物的 SpawnTransform（位置和旋转）被正确应用
		// 即使已通过 SpawnActorDeferred 创建，也需调用此函数完成生成流程
		Projectile->FinishSpawning(SpawnTransform);
	}
}


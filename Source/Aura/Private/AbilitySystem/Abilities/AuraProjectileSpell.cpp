// CopyRight kang


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
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

		//创建一个GE的实例，并设置给投射物
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());
		Projectile->DamageEffectHandle = SpecHandle;

		
		//确保变换设置被正确应用
		Projectile->FinishSpawning(SpawnTransform);
	}
}


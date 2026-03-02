// CopyRight kang


#include "Actor/AuraFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	//目标未继承战斗接口，返回
	if(!OtherActor->Implements<UCombatInterface>()) return;
	
	// //播放击中特效 当前服务器端未调用销毁时，直接播放
	// if(!bHit) PlayImpact();
	
	//在重叠后，销毁自身
	if(HasAuthority()) //是否为服务器，如果服务器端，将执行应用GE，并通过GE的自动同步到客户端
	{
		//为目标应用GE
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// //死亡冲击的力度和方向
			// DamageEffectParams.DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			
			//通过配置项应用给目标ASC
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}


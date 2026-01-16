// CopyRight kang


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystemComponent.h"

void UAuraGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	//通过句柄获取技能实例
	FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
	//技能实例在激活状态，触发输入事件
	if(AbilitySpec->IsActive())
	{
		//将按下事件复制到服务器和所有相关的客户端
		ActorInfo->AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Handle, ActivationInfo.GetActivationPredictionKey());
	}
}

void UAuraGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	//将抬起事件复制到服务器和所有相关的客户端
	ActorInfo->AbilitySystemComponent->InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Handle, ActivationInfo.GetActivationPredictionKey());
}

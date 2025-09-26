// CopyRight kang


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

class IAbilitySystemInterface;

AAuraEffectActor::AAuraEffectActor()
{
	// 设置当前对象是否每帧调用Tick()
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	/**
	* 默认自己编写从actor身上获取ASC的方式
	* IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor); //判断当前actor是否有技能系统接口
	if(ASCInterface)
	{
		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent(); 
	}
	 */

	//获取ASC
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC == nullptr) return;
	
	check(GameplayEffectClass);
	//创建Effect的句柄 包含了实例化Effect所需数据
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	//设置创建Effect的对象
	EffectContextHandle.AddSourceObject(this);
	//Effect的实例化后的句柄，可以通过此来寻找调用
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	//从句柄中获取到实例的地址，并被应用。
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//从句柄中获取到定义的对象，并判断设置的
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//在是无限时间效果和需要在结束时清除掉时，将效果句柄添加到map
	if(bIsInfinite && InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
}


void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}
}


void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	//添加效果
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}

	//删除效果
	if(InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if(!IsValid(TargetASC)) return;

		//创建存储需要移除的效果句柄存储Key，用于遍历完成后移除效果
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		//循环map内存的数据
		for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			//判断是否ASC相同
			if(TargetASC == HandlePair.Value)
			{
				//通过句柄将效果移除，注意，有可能有多层效果，不能将其它层的效果也移除掉，所以只移除一层
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				//添加到移除列表
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		//遍历完成后，在Map中将移除效果的KeyValue删除
		for(auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}


// CopyRight kang


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Aura/AuraLogChannels.h"
#include "Interaction/PlayerInterface.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	//测试
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_Secondary_Armor.ToString() //标签的文本
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	10.f,
	// 	FColor::Blue,
	// 	FString::Printf(TEXT("Tag: %s"), *GameplayTags.Attributes_Secondary_Armor.ToString())
	// 	);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	// 遍历预定义的启动技能类数组（StartupAbilities）
	// StartupAbilities 是一个 TArray<TSubclassOf<UGameplayAbility>>，通常在角色蓝图或数据配置中设置
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// 创建一个 GameplayAbilitySpec 实例//   - AbilityClass：要授予的技能类//   - Level = 1：技能等级（Aura 中通常默认为 1，可用于后续扩展如升级）
		// 此时 AbilitySpec.Ability 会通过 GAS 内部机制实例化为该类的对象（但尚未激活）
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// 尝试将实例化的技能对象（AbilitySpec.Ability）转换为 UAuraGameplayAbility 类型
		// UAuraGameplayAbility 是项目中自定义的基类，用于统一管理技能行为和属性
		if (const UAuraGameplayAbility* AbilityBase = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// 将该技能的 StartupInputTag 添加到 AbilitySpec 的动态来源标签集合中
			// StartupInputTag 是 UAuraGameplayAbility 中定义的一个 FGameplayTag 成员
			// GetDynamicSpecSourceTags() 返回的是可修改的 FGameplayTagContainer 引用
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityBase->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Equipped);
			// AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityBase->TestGameAbilityStatus);
			
			// GiveAbility() 仅注册技能，不会立即激活它,将该技能授予给当前的 AbilitySystemComponent（即角色）,技能将在后续输入触发（如 AbilityInputTagHold）时被激活
			GiveAbility(AbilitySpec);

			// 注释掉的代码：GiveAbilityAndActivateOnce(AbilitySpec)
			// 如果取消注释，会在授予技能的同时立即激活一次（适用于被动触发或开场技能）
		}
	}
	bStartupAbilitiesGiven = true;
	AbilityGivenDelegate.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbilityAndActivateOnce(AbilitySpec); //应用技能并激活一次
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	// 检查传入的 InputTag 是否有效（非空且已注册到 GameplayTag 注册表）
	if(!InputTag.IsValid()) return;

	
	for(auto AbilitySpec : GetActivatableAbilities())
	{
		if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			TryActivateAbility(AbilitySpec.Handle);
		}
	}	
}


// 当某个输入标签（InputTag）被持续按住时调用此函数
// 通常由输入系统（如 Enhanced Input）在按键按下并持续按住时触发
void UAuraAbilitySystemComponent::AbilityInputTagHold(const FGameplayTag& InputTag)
{
	// 检查传入的 InputTag 是否有效（非空且已注册到 GameplayTag 注册表）
	if(!InputTag.IsValid()) return;

	// 遍历所有当前可激活的技能（Abilities）
	// GetActivatableAbilities() 返回一个包含 FGameplayAbilitySpec 对象的容器（通常是 TArray）
	for(auto AbilitySpec : GetActivatableAbilities())
	{
		// 注意：此处使用的是 AbilitySpec.GetDynamicSpecSourceTags()
		// 而不是 DynamicAbilityTags（这是 Aura 项目中的一个常见设计差异）
		// GetDynamicSpecSourceTags() 返回的是该技能实例在运行时动态绑定的“来源标签”集合
		// 在 Aura 中，通常用它来关联输入标签（Input Tags）与技能
		if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			
			// AbilitySpecInputPressed(AbilitySpec);
			// if(!AbilitySpec.IsActive())
			// {
			// 	TryActivateAbility(AbilitySpec.Handle);
			// }
			
		}
	}	
}

// 当某个输入标签（InputTag）被释放时调用此函数
// 通常由输入系统在按键松开时触发
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	// 同样先验证 InputTag 是否有效
	if(!InputTag.IsValid()) return;

	// 遍历所有当前可激活的技能
	for(auto AbilitySpec : GetActivatableAbilities())
	{
		// 再次通过 GetDynamicSpecSourceTags() 检查该技能是否绑定到此 InputTag
		if(AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			// 通知 GAS：该技能对应的输入已被释放
			// 这会触发技能内部的 InputReleased() 函数
			// 常用于结束蓄力、触发释放效果（如射出箭矢、释放火球等）
			AbilitySpecInputReleased(AbilitySpec);
			
		}
	}
}

void UAuraAbilitySystemComponent::


ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); //使用域锁将此作用域this的内容锁定（无法修改），在遍历结束时解锁，保证线程安全
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec)) //运行绑定在技能实例上的委托，如果失败返回false
		{
			UE_LOG(LogAura, Error, TEXT("在函数[%hs]运行委托失败"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(AbilitySpec.Ability)
	{
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags) //获取设置的所有的技能标签并遍历
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) //判断当前标签是否包含"Abilities"名称
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags()) //从技能实例的动态标签容器中遍历所有标签
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Attributes.InputTag")))) //查找标签中是否设置以输入标签开头的标签
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags()) //从技能实例的动态标签容器中遍历所有标签
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))) //查找标签中是否设置以输入标签开头的标签
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this); //域锁
	//遍历已经应用的技能
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UAuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	//从GameMode获取到技能配置数据
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for(const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if(!Info.AbilityTag.IsValid()) continue; //如果没有技能标签，取消执行
		if(Level < Info.LevelRequirement) continue; //如果当期等级未达到所需等级，取消执行
		//判断ASC中是否已存在当前技能实例
		if(GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			//如果没有技能实例，将应用一个新的技能实例
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FAuraGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec); //设置当前技能立即复制到每个客户端
			ClientUpdateAbilityStatus(Info.AbilityTag, FAuraGameplayTags::Get().Abilities_Status_Eligible);
		}
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilityGivenDelegate.Broadcast();
	}
}

void UAuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	//判断Avatar是否基础角色接口
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		//判断是否用于可分配点数
		if(IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag); //调用服务器升级属性
		}
	}

}

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag);
}

void UAuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload; //创建一个事件数据
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;

	//向自身发送事件，通过被动技能接收属性加点
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);

	//判断Avatar是否基础角色接口
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1); //减少一点可分配属性点
	}

}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
		// GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
		FGameplayTagContainer TagContainer;
		EffectSpec.GetAllAssetTags(TagContainer);
	
		EffectAssetTags.Broadcast(TagContainer);
}


// void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation (UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
// {
// 	// GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
// 	FGameplayTagContainer TagContainer;
// 	EffectSpec.GetAllAssetTags(TagContainer);
//
// 	EffectAssetTags.Broadcast(TagContainer);
// }

// CopyRight kang


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	//测试
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	//GameplayTags.Attributes_Secondary_Armor.ToString() //标签的文本
	GEngine->AddOnScreenDebugMessage(
		-1,
		10.f,
		FColor::Blue,
		FString::Printf(TEXT("Tag: %s"), *GameplayTags.Attributes_Secondary_Armor.ToString())
		);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if(const UAuraGameplayAbility* AbilityBase = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityBase->StartupInputTag);
			GiveAbility(AbilitySpec); //只应用不激活
			// GiveAbilityAndActivateOnce(AbilitySpec); //应用技能并激活一次
		}
	}
}



// 当某个输入标签（InputTag）被持续按住时调用此函数
// 通常由输入系统（如 Enhanced Input）在按键按下并持续按住时触发
void UAuraAbilitySystemComponent::AbilityInputTagHold(const FGameplayTag& InputTag)
{
	// 检查传入的 InputTag 是否有效（非空且已注册到 GameplayTag 注册表）
	// 若无效，直接退出函数，避免无效操作
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
			// 通知 GAS：该技能对应的输入已被按下
			// 这会触发该技能内部的 InputPressed() 函数（如果重写了的话）
			// 常用于开始蓄力、持续施法等逻辑
			AbilitySpecInputPressed(AbilitySpec);

			// 检查该技能当前是否**未激活**
			// IsActive() 返回 true 表示技能正在运行中（如已成功激活并处于执行状态）
			if(!AbilitySpec.IsActive())
			{
				// 尝试激活该技能
				// 传入 AbilitySpec 的唯一句柄（Handle），GAS 会检查激活条件（如冷却、资源消耗等）
				// 如果满足条件，技能将进入激活流程（调用 ActivateAbility）
				TryActivateAbility(AbilitySpec.Handle);
			}
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


void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	// GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied!"));
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);
}

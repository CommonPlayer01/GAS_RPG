// CopyRight kang


#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attributes
	 */
	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"),
		FString("Increases physical damage")
		);
	
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"),
		FString("Increases magical damage")
		);
	
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"),
		FString("Increases Armor and Armor Penetration")
		);
	
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"),
		FString("Increases Health")
		);

	/*
	 * Secondary Attributes
	 */
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"),
		FString("Maximum amount of Health obtainable")
		);
	
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"),
		FString("Maximum amount of Mana obtainable")
		);
	
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"),
		FString("Reduces damage taken, improves Block Chance")
		);
		
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignored Percentage of enemy Armor, increases Critical Hit Chance")
		);
	
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in half")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit is scored")
		);
	
	
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduces Critical Hit Chance of attacking Enemies")
		);
	
	
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second")
		);
	
	
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second")
		);

	/*
	 * Input Tags
	 */

	
	InitializeInputGameplayTags();
}


void FAuraGameplayTags::InitializeInputGameplayTags()
{
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.LMB"),
		FString("鼠标左键")
		);
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.RMB"),
		FString("鼠标右键")
		);
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.1"),
		FString("键盘1键")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.2"),
		FString("键盘2键")
		);
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.3"),
		FString("键盘3键")
		);
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Attributes.InputTag.4"),
		FString("键盘4键")
		);
	
	GameplayTags.Damage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
	FName("Damage"),
	FString("伤害标签")
	);


	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("受到攻击时，赋予的标签")
	);

}

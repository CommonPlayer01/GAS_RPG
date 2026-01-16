// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * GameplayTags 标签 单例模式
 * 内部包含原生的项目中使用的标签
 */

struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();
	static void InitializeInputGameplayTags();

	/*
	 * Primary Attributes
	 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;

	/*
	 * Secondary Attributes
	 */
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;

	//属性伤害抗性
	FGameplayTag Attributes_Resistance_Fire; //火属性伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Lightning; //雷属性伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Arcane; //魔法伤害抵抗 标签
	FGameplayTag Attributes_Resistance_Physical; //物理伤害抵抗 标签


	FGameplayTag InputTag_LMB; //鼠标左键
	FGameplayTag InputTag_RMB; //鼠标右键
	FGameplayTag InputTag_1; //1键
	FGameplayTag InputTag_2; //2键
	FGameplayTag InputTag_3; //3键
	FGameplayTag InputTag_4; //4键


	FGameplayTag Damage; //伤害 标签
	FGameplayTag Damage_Fire; //火属性伤害 标签
	FGameplayTag Damage_Lightning; //雷属性伤害 标签
	FGameplayTag Damage_Arcane; //魔法伤害 标签
	FGameplayTag Damage_Physical; //物理伤害 标签
	

	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;


	FGameplayTag Effects_HitReact; //受击 标签


	//阻止输入相关事件触发
	FGameplayTag Player_Block_InputPressed; //阻挡键位按下输入
	FGameplayTag Player_Block_InputHold; //阻挡键位悬停输入
	FGameplayTag Player_Block_InputReleased; //阻挡键位抬起输入
	FGameplayTag Player_Block_CursorTrace; //阻挡鼠标拾取事件

	TArray<FGameplayTag> DamageTypes;

private:
	static FAuraGameplayTags GameplayTags;
	
};

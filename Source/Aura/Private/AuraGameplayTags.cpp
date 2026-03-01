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
		FName("InputTag.LMB"),
		FString("鼠标左键")
		);
	
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.RMB"),
		FString("鼠标右键")
		);
	
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.1"),
		FString("键盘1键")
		);
	
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.2"),
		FString("键盘2键")
		);
	
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.3"),
		FString("键盘3键")
		);
	
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.4"),
		FString("键盘4键")
		);

	GameplayTags.InputTag_Passive_1 = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("InputTag.Passive.1"),
		FString("被动技能1")
		);
	
	GameplayTags.InputTag_Passive_2 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("InputTag.Passive.2"),
			FString("被动技能2")
		);

	/*
	 * 伤害类型
	 */
	GameplayTags.Damage = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Damage"),
		FString("伤害标签")
		);
	
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Fire"),
			FString("火属性伤害")
		);
	
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Lightning"),
			FString("雷属性伤害")
		);
	
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Arcane"),
			FString("魔法伤害")
		);
	
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Damage.Physical"),
			FString("物理伤害")
		);

	/* 属性抗性标签 */
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Fire"),
			FString("火属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Lightning"),
			FString("雷属性抗性")
			);
	
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Arcane"),
			FString("魔法伤害抗性")
			);
	
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Resistance.Physical"),
			FString("物理伤害抗性")
		);

	/*
	 * 负面标签注册
	*/
	GameplayTags.Debuff_Burn = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Burn"),
			FString("火属性燃烧负面标签")
			);
	GameplayTags.Debuff_Stun = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Stun"),
			FString("雷属性眩晕负面标签")
			);
	GameplayTags.Debuff_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Arcane"),
			FString("魔法属性负面标签")
			);
	GameplayTags.Debuff_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Physical"),
			FString("物理属性流血负面标签")
			);

	/*
	 * 负面效果配置标签
	*/
	GameplayTags.Debuff_Chance = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Chance"),
			FString("负面效果 触发几率")
			);
	GameplayTags.Debuff_Damage = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Damage"),
			FString("负面效果 伤害")
			);
	GameplayTags.Debuff_Duration = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Duration"),
			FString("负面效果 持续时间")
			);
	GameplayTags.Debuff_Frequency = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Debuff.Frequency"),
			FString("负面效果 触发间隔")
			);


	
	/* 将属性和抗性标签对应 */
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/*
	 * 负面标签和属性抵抗标签对于对应
	*/
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Fire, GameplayTags.Debuff_Burn);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Lightning, GameplayTags.Debuff_Stun);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Arcane, GameplayTags.Debuff_Arcane);
	GameplayTags.DamageTypesToDebuffs.Add(GameplayTags.Damage_Physical, GameplayTags.Debuff_Physical);


	/*
	 * 元属性
	 */
	GameplayTags.Attributes_Meta_IncomingXP = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Attributes.Meta.IncomingXP"),
			FString("经验元属性标签")
			);
	
	GameplayTags.DamageTypes.Add(GameplayTags.Damage_Fire);


	GameplayTags.Effects_HitReact = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Effects.HitReact"),
		FString("受到攻击时，赋予的标签")
	);

	/*
	 * 当前技能状态标签
	*/
	GameplayTags.Abilities_Status_Locked = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Locked"),
			FString("已锁定")
			);
	GameplayTags.Abilities_Status_Eligible = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Eligible"),
			FString("可解锁")
			);
	GameplayTags.Abilities_Status_Unlocked = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Unlocked"),
			FString("已解锁")
			);
	GameplayTags.Abilities_Status_Equipped = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Status.Equipped"),
			FString("已装配")
			);

	/*
	 * 当前技能类型标签
	*/
	GameplayTags.Abilities_Type_Offensive = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.Offensive"),
			FString("主动技能")
			);
	GameplayTags.Abilities_Type_Passive = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.Passive"),
			FString("被动技能")
			);
	GameplayTags.Abilities_Type_None = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Type.None"),
			FString("啥也不是")
			);
	/*
	 * Abilities
	 */
	GameplayTags.Abilities_None = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.None"),
			FString("空白技能标签")
			);

	GameplayTags.Abilities_Summon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
		FName("Abilities.Summon"),
		FString("Summon技能标签")
		);
	
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Attack"),
			FString("攻击技能标签")
		);

	GameplayTags.Abilities_Fire_FireBolt = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Fire.FireBolt"),
		FString("火球术技能标签")
		);

	GameplayTags.Abilities_Lightning_Electrocute = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Lightning.Electrocute"),
			FString("电击技能标签")
			);
	GameplayTags.Abilities_Arcane_ArcaneShards = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Abilities.Arcane.ArcaneShards"),
		FString("奥术技能奥术爆发标签")
		);



	/*
	 * 被动技能
	*/
	GameplayTags.Abilities_Passive_HaloOfProtection = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.HaloOfProtection"),
			FString("守护光环")
			);
	GameplayTags.Abilities_Passive_LifeSiphon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.LifeSiphon"),
			FString("生命自动回复")
			);
	GameplayTags.Abilities_Passive_ManaSiphon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Abilities.Passive.ManaSiphon"),
			FString("蓝量自动回复")
			);
	/*
	 * 冷却
	 */
	
	GameplayTags.Cooldown_Fire_FireBolt = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Cooldown.Fire.FireBolt"),
		FString("火球术冷却标签")
		);
	GameplayTags.Cooldown_Lighting_Electrocute = UGameplayTagsManager::Get()
	.AddNativeGameplayTag(
		FName("Cooldown.Lightning.Electrocute"),
		FString("闪电链冷却标签")
		);


	/*
	 * Montage
	*/
	
	GameplayTags.Montage_Attack_Weapon = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.Weapon"),
			FString("使用武器攻击蒙太奇标签")
			);
	GameplayTags.Montage_Attack_LeftHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.LeftHand"),
			FString("左手攻击蒙太奇标签")
			);
	GameplayTags.Montage_Attack_RightHand = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Montage.Attack.RightHand"),
			FString("右手攻击蒙太奇标签")
		);


	
	/*
	 * 阻止相关鼠标事件的触发标签
	*/
	GameplayTags.Player_Block_InputPressed = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputPressed"),
			FString("阻挡键位按下输入")
			);
	GameplayTags.Player_Block_InputHold = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputHold"),
			FString("阻挡键位悬停输入")
			);
	GameplayTags.Player_Block_InputReleased = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.InputReleased"),
			FString("阻挡键位抬起输入")
			);
	GameplayTags.Player_Block_CursorTrace = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(
			FName("Player.Block.CursorTrace"),
			FString("阻挡鼠标拾取事件")
			);



}

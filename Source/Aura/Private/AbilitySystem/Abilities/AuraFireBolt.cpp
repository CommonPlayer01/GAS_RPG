// CopyRight kang


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"


FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level); //根据等级获取技能伤害
	
	if(Level == 1)
	{
		return FString::Printf(TEXT("<Title>火球术</>\n<Small>等级：1</>\n\n<Default>发射 1 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"), ScaledDamage);
	}
	
	return FString::Printf(TEXT("<Title>火球术</>\n<Small>等级：%i</>\n\n<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"), Level, FMath::Min(Level, NumProjectiles), ScaledDamage);

}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level + 1); //根据等级获取技能伤害
	return FString::Printf(TEXT("<Title>下一等级</>\n<Small>等级：%i</>\n\n<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"), Level, FMath::Min(Level, NumProjectiles), ScaledDamage);

}

FString UAuraFireBolt::GetDescriptionAtLevel(int32 Level, const FString& Title)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// 标题
		"<Title>%s</>\n"

		// 细节
		"<Small>等级：</> <Level>%i</>\n"
		"<Small>技能冷却：</> <Cooldown>%.1f</>\n"
		"<Small>蓝量消耗：</> <ManaCost>%.1f</>\n\n"//%.1f会四舍五入到小数点后一位

		// 技能描述
		"<Default>发射 %i 颗火球，在发生撞击时产生爆炸，并造成</> <Damage>%i</> <Default>点火焰伤害，并有一定几率燃烧。</>"),

		// 动态修改值
		*Title,
		Level,
		Cooldown,
		ManaCost,
		FMath::Min(Level, NumProjectiles),
		Damage);
}

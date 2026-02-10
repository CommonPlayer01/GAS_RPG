// CopyRight kang


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics 
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	
	AuraDamageStatics()
	{
		//这个宏就像是一个快捷方式，它在你的伤害计算类中预留了一个“槽位”，专门用来存放从外界（AttributeSet）抓取过来的数值快照。
		//参数：1.属性集 2.属性名 3.目标还是自身 4.是否设置快照（true为创建时获取，false为应用时获取）
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef );
	
}


void UExecCalc_Damage::DetermineDebuff(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    const FGameplayEffectSpec& Spec,
    FAggregatorEvaluateParameters EvaluationParameters,
    const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& InTagsToDefs) const
{
    // 获取全局单例标签库
    const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

    // ==========================================
    // 1. 遍历伤害类型与 Debuff 的映射关系
    // ==========================================
    // 例如：物理伤害 -> 眩晕，火焰伤害 -> 燃烧
    for (TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
    {
       const FGameplayTag& DamageType = Pair.Key;
       const FGameplayTag& DebuffType = Pair.Value;
       
       // 检查当前 GE Spec 中是否包含该类型的伤害数值
       const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
       
       // 如果伤害大于 0 (使用 -0.5f 是为了处理浮点数精度问题)
       if (TypeDamage > -.5f) 
       {
          // ==========================================
          // 2. 计算有效 Debuff 触发概率 (Effective Chance)
          // ==========================================
          
          // 从 Spec 中获取攻击方设定的基础 Debuff 触发几率
          const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);

          // 捕获目标的对应抗性（例如：火焰抗性也会降低被点燃的几率）
          float TargetDebuffResistance = 0.f;
          const FGameplayTag& ResistanceTag = GameplayTags.DamageTypesToResistances[DamageType];
          ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(InTagsToDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
          TargetDebuffResistance = FMath::Max<float>(TargetDebuffResistance, 0.f);

          // 计算最终概率：基础几率 * (100 - 抗性)% 
          const float EffectiveDebuffChance = SourceDebuffChance * ( 100 - TargetDebuffResistance ) / 100.f;
          
          // 掷骰子随机判定
          const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;

          // ==========================================
          // 3. 若判定成功，存储 Debuff 参数到 Context
          // ==========================================
          if (bDebuff)
          {
             FGameplayEffectContextHandle ContextHandle = Spec.GetContext();

             // 标记 Debuff 触发成功，并记录当前的伤害类型
             UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
             UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);
             
             // 从 Spec 中提取 Debuff 的具体参数：伤害、持续时间、触发频率
             const float DebuffDamage = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
             const float DebuffDuration = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
             const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);
             
             // 将这些参数写入 Context，以便后续在受击方身上产生持续性效果 (DoT)
             UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
             UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
             UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
          }
       }
    }
}



void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// ==========================================
	// 1. 属性捕获定义与标签初始化
	// ==========================================
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
       
	// 将次要属性（Secondary Attributes）映射到捕获定义，以便后续获取其实际值
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	// 将各类元素抗性映射到捕获定义
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
    
	// ==========================================
	// 2. 获取 Source(攻击者) 与 Target(受击者) 上下文
	// ==========================================
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// 通过接口获取双方等级，用于后续从曲线表中查找对应的系数
	int32 SourcePlayerLevel = 1;
	if (SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel = ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetPlayerLevel = 1;
	if (TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel = ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	// 获取 GE 规格（Spec）和上下文，用于读取标签和存储计算状态（如是否暴击）
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// ==========================================
	// 3. Debuff (负面状态) 判定
	// ==========================================
	// 调用外部函数根据伤害类型判定是否施加燃烧、冰冻等状态
	DetermineDebuff(ExecutionParams, Spec, EvaluationParameters, TagsToCaptureDefs);

	// ==========================================
	// 4. 计算基础伤害与元素抗性减免
	// ==========================================
	float Damage = 0.f;
	// 遍历所有定义的伤害类型及其对应的抗性标签
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;
       
		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = TagsToCaptureDefs[ResistanceTag];

		// 获取 SetByCaller 传入的伤害原始数值
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key, false);
		if (DamageTypeValue <= 0.f)
		{
			continue;
		}
       
		// 捕获目标的抗性值并限制在 0-100 之间
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);

		// 应用抗性百分比减免
		DamageTypeValue *= ( 100.f - Resistance ) / 100.f;

		   // // --- 特殊处理：径向伤害（AOE 衰减） ---
		   // if (UAuraAbilitySystemLibrary::IsRadialDamage(EffectContextHandle))
		   // {
		   //    // 如果是径向伤害，我们需要利用虚幻原生的 ApplyRadialDamageWithFalloff 来计算物理距离上的伤害衰减
		   //    if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(TargetAvatar))
		   //    {
		   //       // 绑定 Lambda 表达式到目标的受损委托，捕获原生引擎计算出的最终衰减值
		   //       CombatInterface->GetOnDamageSignature().AddLambda([&](float DamageAmount)
		   //       {
		   //          DamageTypeValue = DamageAmount;
		   //       });
		   //    }
		   //    UGameplayStatics::ApplyRadialDamageWithFalloff(
		   //       TargetAvatar,
		   //       DamageTypeValue,
		   //       0.f, // 最小伤害
		   //       UAuraAbilitySystemLibrary::GetRadialDamageOrigin(EffectContextHandle),
		   //       UAuraAbilitySystemLibrary::GetRadialDamageInnerRadius(EffectContextHandle),
		   //       UAuraAbilitySystemLibrary::GetRadialDamageOuterRadius(EffectContextHandle),
		   //       1.f, // 伤害衰减指数
		   //       UDamageType::StaticClass(),
		   //       TArray<AActor*>(), // 忽略的 Actors
		   //       SourceAvatar,
		   //       nullptr);
		   // }
		   
		   Damage += DamageTypeValue;
	}

	// ==========================================
	// 5. 格挡判定 (Block)
	// ==========================================
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.f);

	// 随机概率判定
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	// // 将格挡状态存入上下文以便后期同步或触发特效
	// UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

	// 若格挡成功，伤害减半
	Damage = bBlocked ? Damage / 2.f : Damage;

	// ==========================================
	// 6. 护甲穿透与有效护甲减伤计算
	// ==========================================
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.f);

	// 获取数据资产中的系数曲线
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	// 护甲穿透逻辑：降低目标的有效护甲百分比
	const float EffectiveArmor = TargetArmor * ( 100 - SourceArmorPenetration * ArmorPenetrationCoefficient ) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetPlayerLevel);

	// 最终应用护甲对伤害的减免百分比
	Damage *= ( 100 - EffectiveArmor * EffectiveArmorCoefficient ) / 100.f;

	// ==========================================
	// 7. 暴击判定 (Critical Hit)
	// ==========================================
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.f);

	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetPlayerLevel);

	// 有效暴击率 = 来源暴击率 - 目标暴击抵抗 * 系数
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1, 100) < EffectiveCriticalHitChance;

	// 记录暴击状态
	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// 暴击公式：翻倍并加上额外的暴击伤害
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

	// ==========================================
	// 8. 输出计算结果
	// ==========================================
	// 创建一个属性修改器，将计算出的 Damage 以“加法”形式修改目标的 IncomingDamage 属性
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

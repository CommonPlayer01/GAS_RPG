#pragma once //预处理指令 确保这个头文件只被包含（include）一次，防止重复定义。

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;


USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr; //当前场景上下文对象

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr; //需要应用的GE的类

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent; //源ASC

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent; //目标ASC

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType; //技能造成的多种伤害和伤害类型

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f; //技能等级

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DebuffDamageType = FGameplayTag(); //负面效果伤害类型

	UPROPERTY(BlueprintReadWrite)
	float DebuffChance = 0.f; //触发负面效果概率

	UPROPERTY(BlueprintReadWrite)
	float DebuffDamage = 0.f; //负面效果伤害

	UPROPERTY(BlueprintReadWrite)
	float DebuffDuration = 0.f; //负面效果持续时间

	UPROPERTY(BlueprintReadWrite)
	float DebuffFrequency = 0.f; //负面效果触发频率
};




USTRUCT(BlueprintType) //在蓝图中可作为类型使用
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY() //宏 自动生成构造函数、析构函数、拷贝构造函数等

public:

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; }
	float GetDebuffDamage() const { return DebuffDamage; }
	float GetDebuffDuration() const { return DebuffDuration; }
	float GetDebuffFrequency() const { return DebuffFrequency; }
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; }
	TSharedPtr<FGameplayTag> GetDebuffDamageType() const
	{
		return DebuffDamageType;
	}


	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsSuccessfulDebuff(bool bInIsDebuff) { bIsSuccessfulDebuff = bInIsDebuff; }
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; }
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; }
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; }
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) { DamageType = InDamageType; }
	void SetDebuffDamageType(TSharedPtr<FGameplayTag> InDamageType) { DebuffDamageType = InDamageType; }
	
	/** 返回用于序列化的实际结构体 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	/** 创建一个副本，用于后续网络复制或者后续修改 */
	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this; //WithCopy 设置为true，就可以通过赋值操作进行拷贝
		if (GetHitResult())
		{
			// 深拷贝 hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}
	
	/** 用于序列化类的参数 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;
protected:

	UPROPERTY()
	bool bIsBlockedHit = false; //格挡

	UPROPERTY()
	bool bIsCriticalHit = false; //暴击

	UPROPERTY()
	bool bIsSuccessfulDebuff = false; //成功应用负面效果
	
	UPROPERTY()
	float DebuffDamage = 0.f; //负面效果每次造成的伤害

	UPROPERTY()
	float DebuffDuration = 0.f; //负面效果持续时间

	UPROPERTY()
	float DebuffFrequency = 0.f; //负面效果触发频率间隔

	TSharedPtr<FGameplayTag> DamageType; //负面效果的伤害类型

	TSharedPtr<FGameplayTag> DebuffDamageType; //负面效果的伤害类型
};

template<>
struct TStructOpsTypeTraits< FAuraGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FAuraGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};



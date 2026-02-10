#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    // 1. 初始化位掩码，用于记录哪些数据需要被序列化
    uint32 RepBits = 0;
    
    // 如果是“保存”状态（发送端/服务器端）
    if (Ar.IsSaving())
    {
       // 检查各个属性是否有值，如果有，则在 RepBits 对应的位置标记为 1
       if (bReplicateInstigator && Instigator.IsValid()) RepBits |= 1 << 0;
       if (bReplicateEffectCauser && EffectCauser.IsValid()) RepBits |= 1 << 1;
       if (AbilityCDO.IsValid()) RepBits |= 1 << 2;
       if (bReplicateSourceObject && SourceObject.IsValid()) RepBits |= 1 << 3;
       if (Actors.Num() > 0) RepBits |= 1 << 4;
       if (HitResult.IsValid()) RepBits |= 1 << 5;
       if (bHasWorldOrigin) RepBits |= 1 << 6;

       // --- 自定义逻辑开始 ---
       if(bIsBlockedHit) RepBits |= 1 << 7;      // 标记是否格挡
       if(bIsCriticalHit) RepBits |= 1 << 8;     // 标记是否暴击
       if (bIsSuccessfulDebuff) RepBits |= 1 << 9; // 标记是否成功施加 Debuff
       if (DebuffDamage > 0.f) RepBits |= 1 << 10;
       if (DebuffDuration > 0.f) RepBits |= 1 << 11;
       if (DebuffFrequency > 0.f) RepBits |= 1 << 12;
       if (DamageType.IsValid()) RepBits |= 1 << 13; // 伤害类型标签
    }

    // 2. 将位掩码发送出去（告知接收端：我总共用了 14 位来描述后续数据）
    Ar.SerializeBits(&RepBits, 14);

    // 3. 根据位掩码的标记，按顺序进行真正的读/写操作
    if (RepBits & (1 << 0)) Ar << Instigator;
    if (RepBits & (1 << 1)) Ar << EffectCauser;
    if (RepBits & (1 << 2)) Ar << AbilityCDO;
    if (RepBits & (1 << 3)) Ar << SourceObject;
    
    // 特殊处理数组：限制最大长度为 31 以防止恶意溢出
    if (RepBits & (1 << 4)) SafeNetSerializeTArray_Default<31>(Ar, Actors);

    // 处理结构体指针（如 HitResult）
    if (RepBits & (1 << 5))
    {
       if (Ar.IsLoading()) // 如果是接收端，先分配内存
       {
          if (!HitResult.IsValid()) HitResult = TSharedPtr<FHitResult>(new FHitResult());
       }
       HitResult->NetSerialize(Ar, Map, bOutSuccess);
    }
    
    // 处理坐标原点
    if (RepBits & (1 << 6))
    {
       Ar << WorldOrigin;
       bHasWorldOrigin = true;
    }
    else { bHasWorldOrigin = false; }

    // --- 自定义属性的反序列化/序列化 ---
    if (RepBits & (1 << 7)) Ar << bIsBlockedHit;
    if (RepBits & (1 << 8)) Ar << bIsCriticalHit;
    if (RepBits & (1 << 9)) Ar << bIsSuccessfulDebuff;
    if (RepBits & (1 << 10)) Ar << DebuffDamage;
    if (RepBits & (1 << 11)) Ar << DebuffDuration;
    if (RepBits & (1 << 12)) Ar << DebuffFrequency;
    
    if (RepBits & (1 << 13))
    {
       if (Ar.IsLoading()) // 接收端对共享指针进行合法性检查和初始化
       {
          if (!DamageType.IsValid()) DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
       }
       DamageType->NetSerialize(Ar, Map, bOutSuccess);
    }

    // 4. 接收完成后，初始化相关组件引用（如 ASC）
    if (Ar.IsLoading())
    {
       AddInstigator(Instigator.Get(), EffectCauser.Get());
    }  

    bOutSuccess = true;
    return true;
}


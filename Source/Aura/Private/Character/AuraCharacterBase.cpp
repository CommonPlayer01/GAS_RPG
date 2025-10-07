// CopyRight kang


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//设置角色不会和相机碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PROJECTILE, ECR_Overlap); //设置碰撞体可以和发射物产生重叠
	GetMesh()->SetGenerateOverlapEvents(true); //设置碰撞体可以生成重叠事件
	GetMesh()->SetGenerateOverlapEvents(false); //设置碰撞体可以生成重叠事件
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

// 向自身应用一个 GameplayEffect（例如增益、减益、属性初始化等）
void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	// 创建一个 GameplayEffect 上下文句柄，用于携带额外信息（如来源、预测键等）
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();

	// 将当前角色（this）标记为该效果的“来源对象”，便于后续追踪（如死亡时移除来源效果）
	ContextHandle.AddSourceObject(this);

	// 基于指定的效果类、等级和上下文，创建一个传出的 GameplayEffect 规格（Spec）
	// Spec 包含了最终计算后的修饰器（Modifiers）、持续时间、等级等数据
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);

	// 将该效果规格应用到目标（此处目标是自身，即 GetAbilitySystemComponent()）
	// 注意：*SpecHandle.Data.Get() 解引用得到实际的 FGameplayEffectSpec 对象
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}




void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());
	if(!HasAuthority()) return; //查询是否拥有网络权限，应用技能需要添加给服务器

	ASC->AddCharacterAbilities(StartupAbilities);

}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}






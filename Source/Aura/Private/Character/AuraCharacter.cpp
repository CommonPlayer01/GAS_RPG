// CopyRight kang


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true; //设置为true，角色将朝移动的方向旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f); //旋转速率
	GetCharacterMovement()->bConstrainToPlane = true; //约束到平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true; //设置了上面一项为true，且此项设置为true，则在开始时与地面对其

	//设置使用控制器选择Pitch，Yaw，Roll的值。
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//设置玩家职业
	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
	AddCharacterAbilities();

	//设置OwnerActor的Controller
	SetOwner(NewController);
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游
	PlayerStateBase->AddToXP(InXP);
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	return PlayerStateBase->GetPlayerLevel();

}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase);
	//从playerState获取ASC和AS
	AbilitySystemComponent = PlayerStateBase->GetAbilitySystemComponent();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = PlayerStateBase->GetAttributeSet();
	//初始化ASC
	AbilitySystemComponent->InitAbilityActorInfo(PlayerStateBase, this);

	//获取PC
	if(AAuraPlayerController* PlayerControllerBase = Cast<AAuraPlayerController>(GetController()))
	{
		if(AAuraHUD* HUD = Cast<AAuraHUD>(PlayerControllerBase->GetHUD()))
		{
			HUD->InitOverlay(PlayerControllerBase, PlayerStateBase, AbilitySystemComponent, AttributeSet);
		}
	}

	//通过GE初始角色主要属性
	InitializeDefaultAttributes();
}

// CopyRight kang


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	//设置相机
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);	
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>("TopDownCameraComponent");
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	//设置升级特效组件
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent()); //设置附加组件
	LevelUpNiagaraComponent->bAutoActivate = false; //设置不自动激活

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

	LoadProgress();
	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		AuraGameMode->LoadWorldState(GetWorld());
	}
	// AddCharacterAbilities();

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

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles(); //调用播放升级特效
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->GetXP();
}
int32 AAuraCharacter::FindLevelForXP_Implementation(const int32 InXP) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(const int32 Level) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}
int32 AAuraCharacter::GetSpellPointsReward_Implementation(const int32 Level) const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}
void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToLevel(InPlayerLevel);

	if(UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatuses(PlayerStateBase->GetPlayerLevel());
	}
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->GetAttributePoints();
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToAttributePoints(InAttributePoints);
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->GetSpellPoints();
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToSpellPoints(InSpellPoints);
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if(AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if(AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULoadScreenSaveGame* SaveGameData = AuraGameModeBase->RetrieveInGameSaveData();
		if(SaveGameData == nullptr) return;

		//修改存档数据
		SaveGameData->PlayerStartTag = CheckpointTag;

		//修改玩家相关
		if(const AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveGameData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
			SaveGameData->XP = AuraPlayerState->GetXP();
			SaveGameData->AttributePoints = AuraPlayerState->GetAttributePoints();
			SaveGameData->SpellPoints = AuraPlayerState->GetSpellPoints();
		}

		//修改主要属性
		SaveGameData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveGameData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		SaveGameData->bFirstTimeLoadIn = false; //保存完成将第一次加载属性设置为false

		if(!HasAuthority()) return;

		UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		SaveGameData->SavedAbilities.Empty(); //清空数组

		//使用ASC里创建的ForEach函数循环获取角色的技能，并生成技能结构体保存
		FForEachAbility SaveAbilityDelegate;
		SaveAbilityDelegate.BindLambda([this, AuraASC, SaveGameData](const FGameplayAbilitySpec& AbilitySpec)
		{
			//获取技能标签和
			const FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			//创建技能结构体
			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilityStatus = AuraASC->GetStatusTagFromAbilityTag(AbilityTag);
			SavedAbility.AbilityInputTag = AuraASC->GetSlotTagFromAbilityTag(AbilityTag);
			SavedAbility.AbilityType = Info.AbilityType;

			SaveGameData->SavedAbilities.AddUnique(SavedAbility);
		});
		//调用ForEach技能来执行存储到存档
		AuraASC->ForEachAbility(SaveAbilityDelegate);

		
		//保存存档
		AuraGameModeBase->SaveInGameProgressData(SaveGameData);

	}
}


int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase); //检测是否有效，无效会暂停游戏
	return PlayerStateBase->GetPlayerLevel();
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace); //添加标签
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHold); //添加标签
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed); //添加标签
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased); //添加标签
		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}else
	{
		BurnDebuffComponent->Deactivate();
	}
}


void AAuraCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
}

void AAuraCharacter::LoadProgress() const
{
	if(const AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		//获取存档
		ULoadScreenSaveGame* SaveGameData = GameMode->RetrieveInGameSaveData();
		if(SaveGameData == nullptr) return;



		//判断是否为第一次加载存档，如果第一次，属性没有相关内容
		if(SaveGameData->bFirstTimeLoadIn)
		{
			//如果第一次加载存档，使用默认GE初始化主要属性
			InitializeDefaultAttributes();
		
			//初始化角色技能
			AddCharacterAbilities();
		}
		else
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				AuraASC->AddCharacterAbilitiesFromSaveData(SaveGameData);
			}
			//修改玩家相关
			if(AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetLevel(SaveGameData->PlayerLevel);
				AuraPlayerState->SetXP(SaveGameData->XP);
				AuraPlayerState->SetAttributePoints(SaveGameData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveGameData->SpellPoints);
			}
			//如果不是第一次，将通过函数库函数通过存档数据初始化角色属性
			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveGameData);
			
			//初始化角色技能 TODO：还未实现通过存档获取保存的技能，现在测试使用。
			AddCharacterAbilities();
		}
	}

}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* PlayerStateBase = GetPlayerState<AAuraPlayerState>();
	check(PlayerStateBase);
	//从playerState获取ASC和AS
	AbilitySystemComponent = PlayerStateBase->GetAbilitySystemComponent();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	AttributeSet = PlayerStateBase->GetAttributeSet();
	OnASCRegisteredDelegate.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
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

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if(IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator TopCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation(); //获取相机位置和离职特效的朝向
		LevelUpNiagaraComponent->SetWorldRotation(TopCameraRotation); //设置粒子的转向
		LevelUpNiagaraComponent->Activate(true); //激活特效
	}
}

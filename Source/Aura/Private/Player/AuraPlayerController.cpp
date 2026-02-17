// CopyRight kang


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; //是否将数据传送服务器更新

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

}
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(CharacterContext); //判断是否存在

	//从本地角色身上获取到它的子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	/*只有在本地控制的机器并且具有有效的local player时,subsystem才会有效 所以不用check*/
	if (Subsystem)	//检查子系统是否存在
	{
		Subsystem->AddMappingContext(CharacterContext, 0); //可以存在多个操作映射，根据优先级触发
	}
	bShowMouseCursor = true; //游戏中是否显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default; //鼠标光标的样式

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //将鼠标锁定在视口内
	InputModeData.SetHideCursorDuringCapture(false); //鼠标被捕获时是否隐藏
	SetInputMode(InputModeData); //设置给控制器
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();

	AutoRun();

}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlocked, bool bCriticalHit)
{
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //动态创建的组件需要调用注册
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); //先附加到角色身上，使用角色位置
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //然后从角色身上分离，保证在一个位置播放完成动画
		DamageText->SetDamageText(DamageAmount, bBlocked, bCriticalHit); //设置显示的伤害数字
	}

}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	// 尝试获取当前由该控制器（Controller）所控制的 Pawn（通常是角色）
	if (APawn* ControlledPawn = GetPawn())
	{
		// 1. 找到角色当前位置在样条（Spline）上最近的点（世界坐标）
		//    Spline 是一个 USplineComponent 指针，通常在类中预先定义并绑定到场景中的样条组件
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(),  // 当前 Pawn 的世界位置
			ESplineCoordinateSpace::World        // 使用世界空间
		);

		// 2. 获取样条在该最近点处的切线方向（即样条前进方向），同样在世界坐标系中
		//    这个方向向量是单位向量（已归一化），表示角色应朝哪个方向移动
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline,                    // 样条上的点
			ESplineCoordinateSpace::World        // 世界空间
		);

		// 3. 向 Pawn 的移动组件（如 CharacterMovementComponent）添加移动输入
		//    AddMovementInput(Direction) 会让角色朝 Direction 方向移动（基于当前移动速度）
		//    注意：这依赖于 Pawn 是 ACharacter 或具有支持 AddMovementInput 的移动组件
		ControlledPawn->AddMovementInput(Direction);

		// 4. 计算当前样条上最近点与目标终点（CachedDestination）之间的距离
		//    CachedDestination 通常是在开始自动运行时缓存的目标位置（例如玩家点击的地面位置）
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();

		// 5. 如果角色已经足够接近目标点（在允许的误差半径内）
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			// 停止自动运行：设置标志位为 false，通常会停止后续的自动移动逻辑
			bAutoRunning = false;
		}
	}
}



void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent); //获取到增强输入组件

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move); //绑定移动事件

	//绑定Shift按键事件
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ThisClass::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ThisClass::ShiftReleased);
	
	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHold);

}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	//方向控制，如果阻止了按住事件，将不再执行
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHold))
	{
		return;
	}
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>(); //获取输入操作的2维向量值
	const FRotator Rotation = GetControlRotation(); //获取控制器旋转
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); //通过控制器的垂直朝向创建一个旋转值，忽略上下朝向和左右朝向

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //获取世界坐标系下向前的值，-1到1
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //获取世界坐标系下向右的值，-1到1

	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
//鼠标位置追踪
{
	//判断当前事件是否被阻挡，如果事件被阻挡，则清除相关内容
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if(ThisActor) ThisActor->UnHighlightActor();
		if(LastActor) LastActor->UnHighlightActor();
		ThisActor = nullptr; 
		LastActor = nullptr;
		return;
	}


	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit); //获取可视的鼠标命中结果
	if(!CursorHit.bBlockingHit) return; //如果未命中直接返回

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	 * 射线拾取后，会出现的几种情况
	 * 1. LastActor is null   ThisActor is null 不需要任何操作
	 * 2. LastActor is null   ThisActor is valid 高亮ThisActor
	 * 3. LastActor is valid   ThisActor is null 取消高亮LastActor
	 * 4. LastActor is valid   ThisActor is valid LastActor ！= ThisActor 取消高亮LastActor 高亮ThisActor
	 * 5. LastActor is valid   ThisActor is valid LastActor == ThisActor 不需要任何操作
	 */
	
	if(ThisActor != LastActor)
	{
		if(ThisActor) ThisActor->HighlightActor();
		if(LastActor) LastActor->UnHighlightActor();
	}
}


void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	//处理判断按下事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor != nullptr; //ThisActor为鼠标悬停在敌人身上才会有值
		bAutoRunning = false;
		FollowTime = 0.f; //重置统计的时间

		if(bTargeting || bShiftKeyDown)
		{
			if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
		}
	}else
	{
		if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	//处理判断抬起事件是否被阻挡
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// if(GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	
	
	if(!bTargeting && !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn();
		if(FollowTime <= ShortPressThreshold)
		{
			
			if(UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints(); //清除样条内现有的点
				for(const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World); //将新的位置添加到样条曲线中
					// DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f); //点击后debug调试
				}
				if (NavPath->PathPoints.Num() == 0) return;
				//自动寻路将最终目的地设置为导航的终点，方便停止导航
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true; //设置当前正常自动寻路状态，将在tick中更新位置
			}
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
		}
		FollowTime = 0.f;
		bTargeting = false;	
	}
}

void AAuraPlayerController::AbilityInputTagHold(const  FGameplayTag InputTag)
{
	//通过标签阻止悬停事件的触发
	if(GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHold))
	{
		return;
	}
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHold(InputTag);
		}
		return;
	}

	if(bTargeting || bShiftKeyDown)
	{
		// if(GetASC()) GetASC()->AbilityInputTagHold(InputTag);
		return;
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds(); //统计悬停时间来判断是否为点击

		if(CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint; //获取鼠标拾取位置
		}

		if(APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}

	return AuraAbilitySystemComponent;
}





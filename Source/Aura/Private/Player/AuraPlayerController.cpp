// CopyRight kang


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; //是否将数据传送服务器更新
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(CharacterContext); //判断是否存在

	//从本地角色身上获取到它的子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem); //检查子系统是否存在
	Subsystem->AddMappingContext(CharacterContext, 0); //可以存在多个操作映射，根据优先级触发

	bShowMouseCursor = true; //游戏中是否显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default; //鼠标光标的样式

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //将鼠标锁定在视口内
	InputModeData.SetHideCursorDuringCapture(false); //鼠标被捕获时是否隐藏
	SetInputMode(InputModeData); //设置给控制器
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent); //获取到增强输入组件

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move); //绑定移动事件
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
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
	FHitResult CursorHit;
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

	if(LastActor == nullptr)
	{
		if(ThisActor != nullptr)
		{
			//case 2
			ThisActor->HighlightActor();
		} // else case 1
	}
	else
	{
		if(ThisActor == nullptr)
		{
			//case 3
			LastActor->UnHighlightActor();
		}
		else
		{
			if(LastActor != ThisActor)
			{
				//case 4
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			} //else case 5
		}
	}
	
}





// CopyRight kang


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValues()
{
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	10.f,
	// 	FColor::Blue,
	// 	FString::Printf(TEXT("UAuraWidgetController::BroadcastInitialValues()"))
	// 	);
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}

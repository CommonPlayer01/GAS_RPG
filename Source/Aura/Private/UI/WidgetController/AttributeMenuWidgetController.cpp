// CopyRight kang


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		10.f,
		FColor::Blue,
		FString::Printf(TEXT("UAttributeMenuWidgetController::BroadcastInitialValues()"))
		);
	
	
	const UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet);

	check(AttributeInfo);

	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength);
	Info.AttributeValue = AS->GetStrength();
	AttributeInfoDelegate.Broadcast(Info);
	
	
	

	Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Intelligence);
	Info.AttributeValue = AS->GetIntelligence();
	AttributeInfoDelegate.Broadcast(Info);

}

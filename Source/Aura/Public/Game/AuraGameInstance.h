// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FName PlayerStartTag = FName();

	UPROPERTY(EditDefaultsOnly)
	FString LoadSlotName = FString();

	UPROPERTY(EditDefaultsOnly)
	int32 LoadSlotIndex = 0;
};

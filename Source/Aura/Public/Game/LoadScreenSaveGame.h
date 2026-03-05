// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"


//当前存档可以显示的用户控件的枚举
UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};
/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	//存档名称
	UPROPERTY()
	FString SlotName = FString();

	//存档索引
	UPROPERTY()
	int32 SlotIndex = 0;

	//玩家姓名
	UPROPERTY()
	FString PlayerName = FString("Default Name");

	//当前存档进入存档界面时，默认显示的用户界面
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = Vacant;

	//玩家姓名
	UPROPERTY()
	FString MapName = FString("Default Map Name");
};

// CopyRight kang

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
//角色职业类型的枚举
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Elementalist, //法师
	Warrior, //战士
	Ranger //游侠
};


USTRUCT()
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};



/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	//通过枚举获取对应的初始化类
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
	
};

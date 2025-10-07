// CopyRight kang


#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"
UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);

	UAuraAssetManager* MyAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();

	//如果使用TargetData，必须开启此项
	UAbilitySystemGlobals::Get().InitGlobalData();
}




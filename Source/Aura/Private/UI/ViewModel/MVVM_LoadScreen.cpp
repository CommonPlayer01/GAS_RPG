// CopyRight kang


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::SetWidgetName(const FString& InSlotName)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(WidgetName, InSlotName))
	{
		// UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent); //通过宏调用其它函数的广播
	}
}

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName("LoadSlot_0");
	LoadSlots.Add(0, LoadSlot_0);
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName("LoadSlot_1");
	LoadSlots.Add(1, LoadSlot_1);
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName("LoadSlot_2");
	LoadSlots.Add(2, LoadSlot_2);

}


UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(const int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	LoadSlots[Slot]->SetPlayerName(EnterName); //修改MVVM上存储的角色名称
	LoadSlots[Slot]->LoadSlotStatus = Taken; //修改进入界面为加载界面
	AuraGameMode->SaveSlotData(LoadSlots[Slot], Slot); //保存数据
	LoadSlots[Slot]->InitializeSlot(); //调用初始化
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for(const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		if (LoadSlot.Key == Slot)
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(false);
		}else
		{
			LoadSlot.Value->EnableSelectSlotButton.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::LoadData()
{
	//获取到加载存档界面的GameMode
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	//遍历映射，获取对应存档
	for(const TTuple<int32, UMVVM_LoadSlot*> Slot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveGame = AuraGameMode->GetSaveSlotData(Slot.Value->GetSlotName(), Slot.Key);

		//获取存档数据
		const FString PlayerName = SaveGame->PlayerName;
		const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveGame->SaveSlotStatus;

		//设置存档视图模型数据
		Slot.Value->SetPlayerName(PlayerName);
		Slot.Value->LoadSlotStatus = SaveSlotStatus;

		//调用视图模型初始化
		Slot.Value->InitializeSlot();
	}
}

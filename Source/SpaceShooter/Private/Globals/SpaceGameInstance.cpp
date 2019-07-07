// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Globals/SpaceGameInstance.h"
#include "Loot/Items/Weapon.h"
#include "Loot/Creation/PreciseWeaponBlueprint.h"


void USpaceGameInstance::AddCampaignMission(TSubclassOf<UCampaignMissionDescription> CampaignMissionClass,
	FText Name, UCampaignMissionDescription* & CreatedMission)
{
	if (!CampaignMissionClass)
		return;

	CreatedMission = NewObject<UCampaignMissionDescription>(this, FName(*("CampaignMission_" + Name.ToString())));

	if (CreatedMission)
	{
		CreatedMission->Name             = Name;
		CreatedMission->CampaignChapters = TArray<UCampaignChapterDescription*>();

		// After creation, add the mission to the game instance's array of missions.
		CampaignMissions.Add(CreatedMission);
	}
}

void USpaceGameInstance::AddCampaignChapter(TSubclassOf<UCampaignChapterDescription> CampaignChapterClass, FText ChapterName,
	FName LevelName, UGoalDescription* Goal, UCampaignMissionDescription* CampaignMissionToAddTo, UCampaignMissionDescription* & CampaignMission)
{
	if (!CampaignChapterClass || !CampaignMissionToAddTo)
		return;

	FString CampaignChapterObjUniqueName         = "CampaignChapter_" + ChapterName.ToString() + "_" + LevelName.ToString();
	UCampaignChapterDescription* CampaignChapter = NewObject<UCampaignChapterDescription>(this, FName(*CampaignChapterObjUniqueName));

	if (CampaignChapter)
	{
		CampaignChapter->ChapterName = ChapterName;
		CampaignChapter->LevelName   = LevelName;
		CampaignChapter->Goal        = Goal;

		CampaignMissionToAddTo->AddChapter(CampaignChapter);
	}

	// Useful in Blueprints for chained calls of this method.
	CampaignMission = CampaignMissionToAddTo;
}

void USpaceGameInstance::CreateGoalOfTypeDestroyEveryone(const FNpcSpawnRules & NpcSpawnRules,
	TSubclassOf<UUserWidget> GoalWidgetType, UDestroyEveryoneGoalDescription* & GenocideGoal)
{
	GenocideGoal = NewObject<UDestroyEveryoneGoalDescription>(this, FName(*("GenocideGoal_" + FString::FromInt(FMath::Rand()))));

	if (GenocideGoal)
	{
		GenocideGoal->SetWidgetClass(GoalWidgetType);
		GenocideGoal->NpcSpawnRules = NpcSpawnRules;
	}
}

void USpaceGameInstance::SavePlayerWeapons(const TArray<AItem*> & InventoryItems, const FPreparedWeapons & ActiveWeapons,
	int32 EquippedWeaponIndex)
{
	SavedPlayerItemDescriptors.Append(SavedPlayerItemDescriptorsDuringCurrentLevel);
	SavedPlayerItemDescriptorsDuringCurrentLevel.Empty();

	// Clear unnecessary data (other spacecrafts' stuff).
	for (auto Iter = SavedPlayerItemDescriptors.CreateIterator(); Iter; ++Iter)
	{
		if (!IsItemOwnedByPlayer(Iter.Key(), ActiveWeapons, InventoryItems))
		{
			Iter.RemoveCurrent();
		}
	}

	ActiveSlotWeaponDescr1 = SavedPlayerItemDescriptors.Contains(ActiveWeapons.Slot_1)
		? (UPreciseWeaponBlueprint*) SavedPlayerItemDescriptors[ActiveWeapons.Slot_1]
		: nullptr;

	ActiveSlotWeaponDescr2 = SavedPlayerItemDescriptors.Contains(ActiveWeapons.Slot_2)
		? (UPreciseWeaponBlueprint*) SavedPlayerItemDescriptors[ActiveWeapons.Slot_2]
		: nullptr;

	ActiveSlotWeaponDescr3 = SavedPlayerItemDescriptors.Contains(ActiveWeapons.Slot_3)
		? (UPreciseWeaponBlueprint*) SavedPlayerItemDescriptors[ActiveWeapons.Slot_3]
		: nullptr;

	ActiveSlotWeaponDescr4 = SavedPlayerItemDescriptors.Contains(ActiveWeapons.Slot_4)
		? (UPreciseWeaponBlueprint*) SavedPlayerItemDescriptors[ActiveWeapons.Slot_4]
		: nullptr;

	CurrentWeaponActiveSlotIndex = EquippedWeaponIndex;
}

void USpaceGameInstance::ClearPlayerWeapons(bool bOnlyCurrentChapterAcquiredItems)
{
	SavedPlayerItemDescriptorsDuringCurrentLevel.Empty();

	if (!bOnlyCurrentChapterAcquiredItems)
	{
		ActiveSlotWeaponDescr1 = nullptr;
		ActiveSlotWeaponDescr2 = nullptr;
		ActiveSlotWeaponDescr3 = nullptr;
		ActiveSlotWeaponDescr4 = nullptr;

		SavedPlayerItemDescriptors.Empty();

		CurrentWeaponActiveSlotIndex = 0;
	}
}

bool USpaceGameInstance::IsItemOwnedByPlayer(AItem* Item, const FPreparedWeapons & ActiveWeapons, const TArray<AItem*> & InventoryItems) const
{
	if (!Item)	return false;

	if (Item == ActiveWeapons.Slot_1)	return true;
	if (Item == ActiveWeapons.Slot_2)	return true;
	if (Item == ActiveWeapons.Slot_3)	return true;
	if (Item == ActiveWeapons.Slot_4)	return true;

	for (auto InvItem : InventoryItems)
		if (Item == InvItem)
			return true;

	return false;
}

UChapterDescription* USpaceGameInstance::GetNextChapterForCurrentMission()
{
	// First check if we cached a pointer to the next chapter.
	if (NextChapter)
		return NextChapter;

	// Otherwise, search for it and store it in case it exists.

	if (!CurrentMission || !CurrentChapter)
		return nullptr;

	TArray<UChapterDescription*> MissionChapters;
	int32 NextChapterIndex = -1;

	CurrentMission->GetCampaignChaptersAvailable(MissionChapters);

	for (UChapterDescription* Chapter : MissionChapters)
	{
		NextChapterIndex++;

		if (Chapter == CurrentChapter)
		{
			// Make sure the CurrentChapter is not the last one.
			// If it is though, return nullptr, signaling that we are at the end of the mission.

			if (NextChapterIndex < MissionChapters.Num() - 1)
				return NextChapter = MissionChapters[NextChapterIndex + 1];
			else
				return nullptr;
		}
	}

	return nullptr;
}

void USpaceGameInstance::GetPlayerWeapons(TArray<UPreciseWeaponBlueprint*> & WeaponBPs, int32 & EquippedWeaponIndex)
{
	WeaponBPs.Add(ActiveSlotWeaponDescr1);
	WeaponBPs.Add(ActiveSlotWeaponDescr2);
	WeaponBPs.Add(ActiveSlotWeaponDescr3);
	WeaponBPs.Add(ActiveSlotWeaponDescr4);

	for (auto Iter = SavedPlayerItemDescriptors.CreateIterator(); Iter; ++Iter)
		if (UPreciseWeaponBlueprint* WeaponDescr = Cast<UPreciseWeaponBlueprint>(Iter.Value()))
			WeaponBPs.AddUnique(WeaponDescr);

	EquippedWeaponIndex = CurrentWeaponActiveSlotIndex;
}

void USpaceGameInstance::SetCurrentMission(UMissionDescription* NewMission)
{
	CurrentMission = NewMission;
	CurrentChapter = nullptr;
	NextChapter    = nullptr;
}

void USpaceGameInstance::SetCurrentChapter(UChapterDescription* NewChapter)
{
	CurrentChapter = NewChapter;
	NextChapter    = nullptr;
}

UPreciseWeaponBlueprint* USpaceGameInstance::CreateWeaponDescriptor(EWeaponType Type, TSubclassOf<UItemRarity> Rarity,
	FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip, AWeapon* ConstructedWeapon)
{
	FString ObjUniqueName              = "WeaponDescriptor_" + FString::FromInt(FMath::Rand());
	UPreciseWeaponBlueprint* CreatedBP = NewObject<UPreciseWeaponBlueprint>(this, FName(*ObjUniqueName));

	if (CreatedBP)
	{
		CreatedBP->SetType(Type);
		CreatedBP->SetRarity(Rarity);
		CreatedBP->SetBarrel(Barrel);
		CreatedBP->SetBody(Body);
		CreatedBP->SetGrip(Grip);

		SavedPlayerItemDescriptorsDuringCurrentLevel.Add(ConstructedWeapon, CreatedBP);
	}

	return CreatedBP;
}

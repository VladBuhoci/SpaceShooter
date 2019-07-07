// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"
#include "Loot/Creation/WeaponPartsPool.h"
#include "Loot/Creation/LootItemBuilder.h"
#include "GameModes/Gameplay/Missions.h"

#include "CoreMinimal.h"
#include "TextProperty.h"
#include "Engine/GameInstance.h"
#include "SpaceGameInstance.generated.h"

// Forward declarations.
class UWeaponBlueprint;
class UItemRarity;
class AWeapon;
class AItem;

struct FPreparedWeapons;


/**
 * A SpaceGameInstance object lives from the start of the game and until it is shut down.
 * It is a good place to store persistent data.
 */
UCLASS()
class SPACESHOOTER_API USpaceGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	TArray<UCampaignMissionDescription*> CampaignMissions;

	/** Current mission being played. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UMissionDescription* CurrentMission;

	/** Current chapter being played. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UChapterDescription* CurrentChapter;

	/** Next chapter to be played, if any exists. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UChapterDescription* NextChapter;

	UPreciseWeaponBlueprint* ActiveSlotWeaponDescr1;
	UPreciseWeaponBlueprint* ActiveSlotWeaponDescr2;
	UPreciseWeaponBlueprint* ActiveSlotWeaponDescr3;
	UPreciseWeaponBlueprint* ActiveSlotWeaponDescr4;

	/** The array is updated often during a chapter and cleared at every chapter end or on demand. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	TMap<AItem*, UItemBlueprint*> SavedPlayerItemDescriptorsDuringCurrentLevel;

	/** The array is updated before each chapter transition so the player can be supplied when the next level begins. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	TMap<AItem*, UItemBlueprint*> SavedPlayerItemDescriptors;

	/** Index of the prepared slot holding the weapon the player was using before a chapter transition. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	int32 CurrentWeaponActiveSlotIndex;

protected:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void AddCampaignMission(TSubclassOf<UCampaignMissionDescription> CampaignMissionClass, FText Name,
		UCampaignMissionDescription* & CreatedMission);
	
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void AddCampaignChapter(TSubclassOf<UCampaignChapterDescription> CampaignChapterClass, FText ChapterName, FName LevelName,
		UGoalDescription* Goal, UCampaignMissionDescription* CampaignMissionToAddTo, UCampaignMissionDescription* & CampaignMission);

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void CreateGoalOfTypeDestroyEveryone(const FNpcSpawnRules & NpcSpawnRules, TSubclassOf<UUserWidget> GoalWidgetType,
		UDestroyEveryoneGoalDescription* & GenocideGoal);

public:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void SavePlayerWeapons(const TArray<AItem*> & InventoryItems, const FPreparedWeapons & ActiveWeapons, int32 EquippedWeaponIndex);

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void ClearPlayerWeapons(bool bOnlyCurrentChapterAcquiredItems);

private:
	bool IsItemOwnedByPlayer(AItem* Item, const FPreparedWeapons & ActiveWeapons, const TArray<AItem*> & InventoryItems) const;


	/**********************************
				 GETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void GetCampaignMissionsAvailable(TArray<UCampaignMissionDescription*> & Missions) { Missions = CampaignMissions; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UMissionDescription* GetCurrentMission() const { return CurrentMission; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UChapterDescription* GetCurrentChapter() const { return CurrentChapter; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UChapterDescription* GetNextChapterForCurrentMission();

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void GetPlayerWeapons(TArray<UPreciseWeaponBlueprint*> & WeaponBPs, int32 & EquippedWeaponIndex);


	/**********************************
				 SETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void SetCurrentMission(UMissionDescription* NewMission);

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void SetCurrentChapter(UChapterDescription* NewChapter);

	UPreciseWeaponBlueprint* CreateWeaponDescriptor(EWeaponType Type, TSubclassOf<UItemRarity> Rarity,
		FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip, AWeapon* ConstructedWeapon);
};

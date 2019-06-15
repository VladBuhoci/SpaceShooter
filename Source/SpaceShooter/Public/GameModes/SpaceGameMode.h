// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/WeaponBlueprint.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameMode.generated.h"

// Forward declarations.
class ASpacecraftPawn;
class UWeaponPool;
class ULootItemBuilder;


/**
 * The class which defines the Space game rules and core actors that will spawn on map load.
 */
UCLASS()
class SPACESHOOTER_API ASpaceGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	/** Contains every spacecraft currently spawned in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TArray<ASpacecraftPawn*> AllSpacecrafts;

	/** Type of weapon loot pool to create at map load. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UWeaponPool> WeaponPoolClass;

	/** Types of loot builders that will be spawned at game start. They will be accessible via this class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TSet<TSubclassOf<ULootItemBuilder>> LootItemBuilderTypes;

	/** Instance of the global weapon rarities and parts pool. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	UWeaponPool* WeaponPool;

	/**
	 * Map of item builders. Since the game mode reference is available from nearly anywhere,
			it makes this map easy to access.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<ULootItemBuilder>, ULootItemBuilder*> LootItemBuilders;
	
	/**
	 * Base class for loot item builders.
	 * Does nothing when calling its Build() method, but GetLootBuilder() returns it if needed to avoid null reference issues.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	ULootItemBuilder* GenericItemBuilder;

public:
	/** Sets default values. */
	ASpaceGameMode();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:
	void NotifySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft);
	void NotifySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft);

private:
	void FindAllSpacecraftsInWorld();
	void SupplyKnownSpacecraftsInWorld();

	void SupplySpacecraftIfNeeded(ASpacecraftPawn* SpacecraftToSupply);
	void SupplySpacecraftWithStartingWeapons(ASpacecraftPawn* SpacecraftToSupply, TArray<TSubclassOf<UItemBlueprint>> WeaponBlueprints);

	void CreateGlobalWeaponPool();
	void CreateLootBuilders();

	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns a pointer to the loot pool containing all weapon parts, organized by rarities and weapon types. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	UWeaponPool* GetGlobalWeaponPool() const { return WeaponPool; }

	/** Given a type, returns the right loot item builder. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	ULootItemBuilder* GetLootBuilder(TSubclassOf<ULootItemBuilder> Type) const;

	/** Returns all spacecrafts found in the current world. */
	UFUNCTION(BlueprintPure, Category = "Space Game Mode")
	TArray<ASpacecraftPawn*> GetAllSpacecraftsInWorld() const { return AllSpacecrafts; }
};

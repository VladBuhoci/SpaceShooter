// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/PreciseWeaponBlueprint.h"
#include "Globals/SpaceStructs.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameMode.generated.h"

// Forward declarations.
class ASpacecraftPawn;
class ASpaceEnemyPawn;
class UWeaponPool;
class ULootItemBuilder;


/**
 * The class which defines the Space game rules and core actors that will spawn on map load.
 */
UCLASS()
class SPACESHOOTER_API ASpaceGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	/** Contains every spacecraft currently spawned in the world. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TArray<ASpacecraftPawn*> AllSpacecrafts;

private:
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
	 *	it makes this map easy to access.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<ULootItemBuilder>, ULootItemBuilder*> LootItemBuilders;

	bool bInitialSupplyDeliveredToFirstSpacecrafts;
	
public:
	/** Sets default values. */
	ASpaceGameMode();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called when all enemies in the current level have been wiped out. */
	virtual void OnSectorCleared();
	/** Called when the player's spacecraft has been destroyed. */
	virtual void OnPlayerDestroyed();

public:
	void NotifySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft);
	void NotifySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft);

protected:
	virtual void OnPlayerSpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft) {}
	virtual void OnEnemySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft) {}
	virtual void OnEnemySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft) {}

	void FindAllSpacecraftsInWorld();
	void SupplyKnownSpacecraftsInWorld();

	bool IsOnlyPlayerLeft() const;

	void SupplySpacecraftIfNeeded(ASpacecraftPawn* SpacecraftToSupply);
	void SupplySpacecraftWithStartingWeapons(ASpacecraftPawn* SpacecraftToSupply,
		const TArray<TSubclassOf<UWeaponBlueprint>> & WeaponBlueprints);
	void SupplySpacecraftWithStartingWeapons(ASpacecraftPawn* SpacecraftToSupply,
		const TArray<UPreciseWeaponBlueprint*> & WeaponBlueprints);

	void CreateGlobalWeaponPool();
	void CreateLootBuilders();

	ASpaceEnemyPawn* SpawnSpacecraftNPC(TSubclassOf<ASpaceEnemyPawn> SpacecraftClass, bool bIsAggressive,
		const FVector & Location = FVector::ZeroVector, const FRotator & Rotation = FRotator::ZeroRotator,
		const TArray<TSubclassOf<UWeaponBlueprint>> & ReplacingStartingWeaponBlueprints = TArray<TSubclassOf<UWeaponBlueprint>>());

	UFUNCTION(BlueprintCallable, Category = "Space Game Mode", Meta = (DisplayName = "Spawn Spacecraft NPC"))
	ASpaceEnemyPawn* SpawnSpacecraftNPC_BP(TSubclassOf<ASpaceEnemyPawn> SpacecraftClass, bool bIsAggressive,
		const FVector & Location, const FRotator & Rotation,
		const TArray<TSubclassOf<UWeaponBlueprint>> & ReplacingStartingWeaponBlueprints);

	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns a pointer to the loot pool containing all weapon parts, organized by rarities and weapon types. */
	UFUNCTION(BlueprintCallable, Category = "Space Game Mode")
	UWeaponPool* GetGlobalWeaponPool();

	/** Given a type, returns the right loot item builder. */
	UFUNCTION(BlueprintCallable, Category = "Space Game Mode")
	ULootItemBuilder* GetLootBuilder(TSubclassOf<ULootItemBuilder> Type);

	/** Returns all spacecrafts found in the current world. */
	UFUNCTION(BlueprintPure, Category = "Space Game Mode")
	TArray<ASpacecraftPawn*> GetAllSpacecraftsInWorld() const { return AllSpacecrafts; }
};

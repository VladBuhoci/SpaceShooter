// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SubclassOf.h"
#include "SpaceStructs.generated.h"

// Forward declarations.
class UTexture2D;

class ASpaceEnemyPawn;


/**
 * Describes the ammo stock for one particular weapon type.
 */
USTRUCT(BlueprintType)
struct FAmmunitionStock
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons")
	bool bEndlessAmmo = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (EditCondition = "!bEndlessAmmo"))
	int32 CurrentAmmoQuantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (EditCondition = "!bEndlessAmmo"))
	int32 MaxAmmoQuantity;

	FAmmunitionStock()				                   : FAmmunitionStock(0)										{};
	FAmmunitionStock(bool bInfiniteAmmo)			   : bEndlessAmmo(bInfiniteAmmo)								{};
	FAmmunitionStock(int32 CurrMaxAmmo)                : FAmmunitionStock(CurrMaxAmmo, CurrMaxAmmo)					{};
	FAmmunitionStock(int32 CurrentAmmo, int32 MaxAmmo) : CurrentAmmoQuantity(CurrentAmmo), MaxAmmoQuantity(MaxAmmo)	{};
};

/**
 * Used as templates to easily configure common attributes (like Damage, Accuracy etc.) once only.
 */
USTRUCT(BlueprintType)
struct FItemAttributeDescription
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	FText Name;

	FItemAttributeDescription()
		: Icon(nullptr), Name(FText::FromString("-"))
	{}
};

/**
 * Contains the usual game stats (total kills, shots fired, shots hit (defining player's average accuracy) etc.)
 */
USTRUCT(BlueprintType)
struct FLevelOverallStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD")
	int32 TotalKills;
};

/** Pairs a type of NPC with the chance of this type being instantiated and added to the world. */
USTRUCT(BlueprintType)
struct FSpawnableNPC
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	TSubclassOf<ASpaceEnemyPawn> NpcClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	float SpawnChance;
};

/** Describes the rules of spawning for a game mode. */
USTRUCT(BlueprintType)
struct FNpcSpawnRules
{
	GENERATED_USTRUCT_BODY()

	/** Types of NPC spacecrafts that will be spawned in the game as well as their spawn chance (1-100%). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	TArray<FSpawnableNPC> SpawnableNpcClasses;

	/**
	 * Total amount of NPCs to be spawned (includes those NPCs that were already placed in the level at start).
	 * If negative, NPCs will appear endlessly.
	 * If zero, NPCs will never be spawned (does not affect those spacecrafts placed by the level designer).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	int32 TotalNPCsToBeSpawned;

	/** The amount of NPCs spawned in the present (includes those NPCs that were already placed in the level at start). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space Game Mode")
	int32 NPCsSpawnedCurrentCount;

	/** Radius of the circular area (around the player) inside which NPCs will spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	float NPCsSpawnAreaRadius;

	/** Minimum distance between future-spawned NPCs and player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	float NPCsSpawnMinDistance;

	/** Minimum number of NPC spacecrafts that will be added to the map in the upcoming wave. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	int32 NPCsPerWaveMin;

	/** Maximum number of NPC spacecrafts that will be added to the map in the upcoming wave. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	int32 NPCsPerWaveMax;

	/** Minimum time (in seconds) until the next wave begins. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	float TimeUntilNextWaveMin;

	/** Maximum time (in seconds) until the next wave begins. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	float TimeUntilNextWaveMax;

	/** Determines whether NPCs will spawn with the aggressive state set to true or not. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Space Game Mode")
	bool bStartAggressive;

	bool  HasInfiniteWaves()         const { return TotalNPCsToBeSpawned < 0;                                     }
	int32 GetNPCsPerWaveRand()       const { return FMath::RandRange(NPCsPerWaveMin      , NPCsPerWaveMax      ); }
	float GetTimeUntilNextWaveRand() const { return FMath::RandRange(TimeUntilNextWaveMin, TimeUntilNextWaveMax); }
	
	TSubclassOf<ASpaceEnemyPawn> GetNpcClassRand() const
	{
		if (SpawnableNpcClasses.Num() == 0)
			return NULL;

		int32 Chances = 0;

		// Collect all chances.
		for (FSpawnableNPC SpawnableNPC : SpawnableNpcClasses)
			Chances += SpawnableNPC.SpawnChance;

		// Pick a random number from the "pile" (sum of chances).
		int32 LuckyNumber = FMath::RandRange(1, Chances);

		// Find the owner of the interval the lucky number belongs to.
		int32 LuckyIntervalSuperiorLimit = 0;

		for (FSpawnableNPC SpawnableNPC : SpawnableNpcClasses)
		{
			LuckyIntervalSuperiorLimit += SpawnableNPC.SpawnChance;

			if (LuckyNumber <= LuckyIntervalSuperiorLimit)
				return SpawnableNPC.NpcClass;
		}

		return NULL;
	}
};

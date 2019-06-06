// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "LootPool.generated.h"


// Forward declarations.
class ASpaceGameMode;
class ULootDefinition;
class ALootItemBuilder;
class AItem;
class AItemBox;


USTRUCT(BlueprintType)
struct SPACESHOOTER_API FLootDefinitionWrapper
{
	GENERATED_USTRUCT_BODY()

	/** Type of the item to be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool")
	TSubclassOf<AItem> ItemTypeToSpawn;

	/** Type of item box that will contain this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool")
	TSubclassOf<AItemBox> ItemBoxTypeToSpawn;

	/** Type of builder that handles the spawning of this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool")
	TSubclassOf<ALootItemBuilder> BuilderType;
};

USTRUCT(BlueprintType)
struct SPACESHOOTER_API FItemPoolCandidate
{
	GENERATED_USTRUCT_BODY()

	/** Definition of an item or pool of items that can be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool")
	TSubclassOf<ULootDefinition> ItemOrPoolDefinition;

	/**
	 * A value that describes the chance this item/pool has to be picked by the RNG process.
	 * Higher value -> better chance.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool")
	int32 SelectionChance;
};

UCLASS(Blueprintable)
class SPACESHOOTER_API ULootDefinition : public UObject
{
	GENERATED_BODY()

public:
	virtual FLootDefinitionWrapper GetRandomItemDefinition() { return FLootDefinitionWrapper(); }

	virtual UWorld* GetWorld() const override { return GetOuter() != nullptr ? GetOuter()->GetWorld() : nullptr; }
};

UCLASS(Blueprintable)
class SPACESHOOTER_API UItemDefinition : public ULootDefinition
{
	GENERATED_BODY()

	/** Defines the type of item to be spawned and its builder class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool", Meta = (AllowPrivateAccess = "true"))
	FLootDefinitionWrapper LootItemDefinitionWrapper;

public:
	virtual FLootDefinitionWrapper GetRandomItemDefinition() override;
};

UCLASS(Blueprintable)
class SPACESHOOTER_API UItemPoolDefinition : public ULootDefinition
{
	GENERATED_BODY()

	/**
	 * An array of item definitions (which can actually be other item pools).
	 * The RNG process "rolls a die" for each candidate and whoever "wins" will be used for further "rolls",
			until the process reaches the end of the recursive chain of calls, by finding an Item Definition.
	 * Only one of the candidates can be picked for spawning.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool", Meta = (AllowPrivateAccess = "true"))
	TArray<FItemPoolCandidate> Candidates;

public:
	virtual FLootDefinitionWrapper GetRandomItemDefinition() override;
};

UCLASS(Blueprintable)
class SPACESHOOTER_API UItemPoolListDefinition : public UObject
{
	GENERATED_BODY()

	/**
	 * An array of item definitions (which can actually be other item pools).
	 * The RNG process "rolls a die" for each candidate and whoever "wins" will be used for further "rolls",
			until the process reaches the end of the recursive chain of calls, by finding an Item Definition.
	 * All of the candidates can be spawned, or none at all.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool", Meta = (AllowPrivateAccess = "true"))
	TArray<FItemPoolCandidate> Candidates;

	/** How many times should the RNG process iterate through the array of candidates. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Pool", Meta = (AllowPrivateAccess = "true"))
	int32 NumberOfCycles = 1;

public:
	TArray<AItem*> GetRandomItems();
	TArray<AItemBox*> GetRandomItemsWrappedInBoxes();

	virtual UWorld* GetWorld() const override { return GetOuter() != nullptr ? GetOuter()->GetWorld() : nullptr; }

private:
	TArray<ULootDefinition*> GetRandomLootDefinitions();

	/**
	 * Rolls a die.
	 * If the "chance" value happens to be lower than or equal to the generated number from the interval, it returns true.
	 */
	bool IsCandidateAllowedInDeepSearchPhase(float Chance, float IntervalMin = 1.0f, float IntervalMax = 100.0f);

	AItem* BuildItemFromDefinition(const FLootDefinitionWrapper & ItemDefWrapper, ASpaceGameMode* SpaceGameMode, const FTransform & Transform = FTransform::Identity);
	AItemBox* BuildItemBoxFromDefinition(const FLootDefinitionWrapper & ItemDefWrapper, const FTransform & Transform = FTransform::Identity);
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootPool.h"
#include "SpaceGameMode.h"
#include "LootItemBuilder.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"


FLootDefinitionWrapper UItemDefinition::GetRandomItemDefinition()
{
	return LootItemDefinitionWrapper;
}

FLootDefinitionWrapper UItemPoolDefinition::GetRandomItemDefinition()
{
	// Example of how item "lottery tickets" are distributed using their chances.
	// A random number will be chosen from the total chances number (20 in this case).
	// Then we pick up the item this number would belong to and the recursive process goes on.
	// 
	// ==========================================================
	// |Item 0 |    Item 1   |Item 2 |    Item 3    |  Item 4   |
	// |1  2  3|4  5  6  7  8|9 10 11|12 13 14 15 16|17 18 19 20|
	// ==========================================================
	// Let's say ticket #20 is chosen => Item 4 wins and we call GetRandomItem() on it.

	int32 TotalTickets = 0;
	int32 ChosenTicketNumber = 0;
	int32 ChosenIndex = 0;

	// Calculate the total chances from this pool (which would equal to the last number in the "tickets" array.
	for (int32 i = 0; i < Candidates.Num(); i++)
	{
		TotalTickets += Candidates[i].SelectionChance;
	}

	// Pick a "ticket".
	ChosenTicketNumber = FMath::RandRange(1, TotalTickets);

	int32 TicketInterval = 0;

	// Search for the lucky winner (owner of the "ticket" - number)
	for (int32 i = 0; i < Candidates.Num(); i++)
	{
		TicketInterval += Candidates[i].SelectionChance;

		if (ChosenTicketNumber <= TicketInterval)
		{
			ChosenIndex = i;
			break;
		}
	}

	UClass* LootDefClass = Candidates[ChosenIndex].ItemOrPoolDefinition.Get();
	ULootDefinition* LootDef = NewObject<ULootDefinition>(this, LootDefClass);
	
	return LootDef->GetRandomItemDefinition();
}

TArray<AItem*> UItemPoolListDefinition::GetRandomItems()
{
	TArray<AItem*> SpawnedItems;
	
	if (Candidates.Num() > 0)
	{
		UWorld* WorldPtr = GetWorld();

		if (WorldPtr)
		{
			ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));

			if (SpaceGameMode)
			{
				for (int32 i = 0; i < NumberOfCycles; i++)
				{
					// We roll a die for each possible candidate in a cycle.
					// Anyone can win (get spawned), one or more (even all).

					for (FItemPoolCandidate Candidate : Candidates)
					{
						if (IsCandidateAllowedInDeepSearchPhase(Candidate.SelectionChance))
						{
							ULootDefinition* LootDef = NewObject<ULootDefinition>(this, Candidate.ItemOrPoolDefinition);
							
							SpawnedItems.Add(BuildItemFromDefinition(LootDef->GetRandomItemDefinition(), SpaceGameMode));
						}
					}
				}
			}
		}
	}

	return SpawnedItems;
}

// TODO: is this the best way to calculate the chance?
bool UItemPoolListDefinition::IsCandidateAllowedInDeepSearchPhase(float Chance, float IntervalMin, float IntervalMax)
{
	float GeneratedValue = FMath::RandRange(IntervalMin, IntervalMax);

	return Chance <= GeneratedValue;
}

AItem* UItemPoolListDefinition::BuildItemFromDefinition(const FLootDefinitionWrapper & ItemDefWrapper, ASpaceGameMode* SpaceGameMode, const FTransform & Transform)
{
	return SpaceGameMode
		->GetLootBuilder(ItemDefWrapper.BuilderType)
		->Build(ItemDefWrapper.ItemTypeToSpawn, Transform);
}

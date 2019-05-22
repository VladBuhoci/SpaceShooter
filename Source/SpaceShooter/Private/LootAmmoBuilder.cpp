// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootAmmoBuilder.h"
#include "Item.h"

#include "Engine/World.h"


AItem* ALootAmmoBuilder::Build(TSubclassOf<AItem> ItemToBuildType, const FTransform & SpawnedItemTransform)
{
	AItem* SpawnedItem = nullptr;
	UWorld* WorldPtr = GetWorld();

	if (ItemToBuildType && WorldPtr)
	{
		SpawnedItem = WorldPtr->SpawnActor<AItem>(ItemToBuildType, SpawnedItemTransform);
	}

	return SpawnedItem;
}
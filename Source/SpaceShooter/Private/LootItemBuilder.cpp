// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootItemBuilder.h"
#include "Loot/ItemBlueprint.h"
#include "Item.h"


ALootItemBuilder::ALootItemBuilder()
{
	
}

AItem* ALootItemBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	return nullptr;
}

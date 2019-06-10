// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/LootItemBuilder.h"
#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/Items/Item.h"


ULootItemBuilder::ULootItemBuilder()
{
	
}

AItem* ULootItemBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	return nullptr;
}

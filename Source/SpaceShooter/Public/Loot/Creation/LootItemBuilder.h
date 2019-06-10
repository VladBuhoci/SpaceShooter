// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LootItemBuilder.generated.h"

// Forward class declarations.
class UItemBlueprint;
class AItem;


/**
 * NOTE: do not use this! Instead, make use of one of its subclasses. This one is bare-bones and it may cause crashes.
 * Base class of loot item builders.
 * Builders handle the spawning and setting up of loot.
 */
UCLASS()
class SPACESHOOTER_API ULootItemBuilder : public UObject
{
	GENERATED_BODY()
	
public:
	/** Sets defaults values. */
	ULootItemBuilder();

	virtual AItem* Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform);
};

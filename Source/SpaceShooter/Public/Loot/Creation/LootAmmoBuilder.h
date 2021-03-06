// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/LootItemBuilder.h"

#include "CoreMinimal.h"
#include "LootAmmoBuilder.generated.h"


/**
 * Ammunition-type loot item builder.
 */
UCLASS()
class SPACESHOOTER_API ULootAmmoBuilder : public ULootItemBuilder
{
	GENERATED_BODY()
	
public:
	virtual AItem* Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform = FTransform::Identity) override;
};

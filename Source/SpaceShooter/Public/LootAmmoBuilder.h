// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "LootItemBuilder.h"
#include "LootAmmoBuilder.generated.h"


/**
 * Ammunition-type loot item builder.
 */
UCLASS()
class SPACESHOOTER_API ALootAmmoBuilder : public ALootItemBuilder
{
	GENERATED_BODY()
	
public:
	virtual AItem* Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform) override;
};

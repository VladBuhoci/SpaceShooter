// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "LootItemBuilder.h"
#include "LootWeaponBuilder.generated.h"


/**
 * Weapon-type loot item builder.
 */
UCLASS()
class SPACESHOOTER_API ALootWeaponBuilder : public ALootItemBuilder
{
	GENERATED_BODY()
	
public:
	virtual AItem* Build(TSubclassOf<AItem> ItemToBuildType, const FTransform & SpawnedItemTransform) override;
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/LootItemBuilder.h"

#include "CoreMinimal.h"
#include "LootWeaponBuilder.generated.h"


/**
 * Weapon-type loot item builder.
 */
UCLASS()
class SPACESHOOTER_API ULootWeaponBuilder : public ULootItemBuilder
{
	GENERATED_BODY()
	
public:
	virtual AItem* Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform) override;
};

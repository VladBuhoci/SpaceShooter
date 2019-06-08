// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootAmmoBuilder.h"
#include "Loot/ItemBlueprint.h"
#include "Loot/AmmunitionBlueprint.h"
#include "Item.h"
#include "AmmunitionPile.h"

#include "Engine/World.h"


AItem* ALootAmmoBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	AAmmunitionPile* SpawnedAmmoPile = nullptr;
	UWorld* WorldPtr = GetWorld();

	if (ItemToBuildBlueprint && WorldPtr)
	{
		UAmmunitionBlueprint* AmmoBP = NewObject<UAmmunitionBlueprint>(this, ItemToBuildBlueprint);

		if (AmmoBP)
		{
			SpawnedAmmoPile = WorldPtr->SpawnActor<AAmmunitionPile>(AAmmunitionPile::StaticClass(), SpawnedItemTransform);
			
			if (SpawnedAmmoPile)
			{
				FText ItemName = AmmoBP->GetItemName();
				FItemAttribute_Float AmmoCountAttr;

				AmmoBP->GetAmmoCountRandomized(AmmoCountAttr);

				SpawnedAmmoPile->SetItemName(ItemName);
				SpawnedAmmoPile->SetItemIcon(AmmoBP->GetItemIcon());
				SpawnedAmmoPile->SetAmmoType(AmmoBP->GetAmmoType());
				SpawnedAmmoPile->SetAmmoCountAttribute(AmmoCountAttr);
			}
		}
	}

	return SpawnedAmmoPile;
}

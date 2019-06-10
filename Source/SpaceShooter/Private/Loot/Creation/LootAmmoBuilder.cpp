// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/LootAmmoBuilder.h"
#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/Creation/AmmunitionBlueprint.h"
#include "Loot/Items/Item.h"
#include "Loot/Items/AmmunitionPile.h"
#include "Loot/ItemRarity.h"

#include "Engine/World.h"


AItem* ULootAmmoBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
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
				UItemRarity* ItemRarity = NewObject<UItemRarity>(this, AmmoBP->GetAmmoRarity());
				FItemAttribute_Float AmmoCountAttr;

				AmmoBP->GetAmmoCountRandomized(AmmoCountAttr);

				SpawnedAmmoPile->SetItemName(ItemName);
				SpawnedAmmoPile->SetItemIcon(AmmoBP->GetItemIcon());
				SpawnedAmmoPile->SetAmmoType(AmmoBP->GetAmmoType());
				SpawnedAmmoPile->SetItemRarity(ItemRarity);
				SpawnedAmmoPile->SetAmmoCountAttribute(AmmoCountAttr);
			}
		}
	}

	return SpawnedAmmoPile;
}

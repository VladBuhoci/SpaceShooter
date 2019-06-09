// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootWeaponBuilder.h"
#include "Loot/ItemBlueprint.h"
#include "Loot/WeaponBlueprint.h"
#include "Item.h"
#include "Weapon.h"

#include "Engine/World.h"


AItem* ULootWeaponBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	AWeapon* SpawnedWeapon = nullptr;
	UWorld* WorldPtr = GetWorld();

	if (ItemToBuildBlueprint && WorldPtr)
	{
		UWeaponBlueprint* WeaponBP = NewObject<UWeaponBlueprint>(this, ItemToBuildBlueprint);

		if (WeaponBP)
		{
			SpawnedWeapon = WorldPtr->SpawnActor<AWeapon>(AWeapon::StaticClass(), SpawnedItemTransform);

			if (SpawnedWeapon)
			{
				FText ItemName = WeaponBP->GetItemName();

				SpawnedWeapon->SetItemName(ItemName);
				SpawnedWeapon->SetItemIcon(WeaponBP->GetItemIcon());
				SpawnedWeapon->SetType(WeaponBP->GetType());
			}
		}
	}

	return SpawnedWeapon;
}


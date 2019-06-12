// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/LootWeaponBuilder.h"
#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/Creation/WeaponBlueprint.h"
#include "Loot/Creation/WeaponPool.h"
#include "Loot/Creation/WeaponPartsPool.h"
#include "Loot/Items/Item.h"
#include "Loot/Items/Weapon.h"
#include "GameModes/SpaceGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"


AItem* ULootWeaponBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	if (!ItemToBuildBlueprint)
		return nullptr;

	AWeapon* SpawnedWeapon = nullptr;
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));
		UWeaponBlueprint* WeaponBP = NewObject<UWeaponBlueprint>(this, ItemToBuildBlueprint);

		if (SpaceGameMode && WeaponBP)
		{
			FWeaponBarrel Barrel;
			FWeaponBody Body;
			FWeaponGrip Grip;

			// Obtain the components to set up the weapon.
			if (SpaceGameMode->GetGlobalWeaponPool()->GetWeaponParts(WeaponBP->GetRarity(), WeaponBP->GetType(), Barrel, Body, Grip))
			{
				// Create the bare-bones weapon.
				SpawnedWeapon = WorldPtr->SpawnActor<AWeapon>(AWeapon::StaticClass(), SpawnedItemTransform);

				if (SpawnedWeapon)
				{
					SetUpWeapon(SpawnedWeapon, WeaponBP, Barrel, Body, Grip);
				}
			}
		}
	}

	return SpawnedWeapon;
}

void ULootWeaponBuilder::SetUpWeapon(AWeapon* Weapon, UWeaponBlueprint* WeaponBP, FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip)
{
	FText ItemName        = FText::Format(FText::FromString("{0} {1} {2}"), Grip.WeaponNamePrefix, Body.WeaponNamePrefix, Barrel.WeaponNameBase);
	UTexture2D* Icon      = WeaponBP->GetItemIcon();
	UItemRarity* Rarity   = NewObject<UItemRarity>(this, WeaponBP->GetRarity());
	EWeaponType Type      = WeaponBP->GetType();
	// ...
	UStaticMesh* BarrelMesh = Barrel.BarrelMesh;
	UStaticMesh* BodyMesh   = Body.BodyMesh;
	UStaticMesh* GripMesh   = Grip.GripMesh;
	// ...
	TSubclassOf<AProjectile> ProjectileClass = Barrel.ProjectileClass;
	// ...

	Weapon->SetItemName(ItemName);
	Weapon->SetItemIcon(Icon);
	Weapon->SetItemRarity(Rarity);
	Weapon->SetType(Type);
	// ...
	Weapon->SetBarrelMesh(BarrelMesh);
	Weapon->SetBodyMesh(BodyMesh);
	Weapon->SetGripMesh(GripMesh);
	// ...
	Weapon->SetProjectileClass(ProjectileClass);
	// ...
}

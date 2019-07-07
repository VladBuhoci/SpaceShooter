// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/ItemRarity.h"
#include "Loot/Creation/WeaponPartsPool.h"
#include "Loot/Creation/WeaponBlueprint.h"

#include "SubclassOf.h"
#include "PreciseWeaponBlueprint.generated.h"


/**
 * Used by the LootWeaponBuilder class to instantiate Weapon objects.
 */
UCLASS(NotBlueprintable)
class SPACESHOOTER_API UPreciseWeaponBlueprint : public UWeaponBlueprint
{
	GENERATED_BODY()
	
private:
	/** Weapon barrel descriptor.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Precise Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	FWeaponBarrel Barrel;
	
	/** Weapon body descriptor.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Precise Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	FWeaponBody Body;

	/** Weapon grip descriptor.  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Precise Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	FWeaponGrip Grip;

public:
	void GetBarrel(FWeaponBarrel & WeaponBarrel) { WeaponBarrel = Barrel; }
	void GetBody(FWeaponBody & WeaponBody) { WeaponBody = Body; }
	void GetGrip(FWeaponGrip & WeaponGrip) { WeaponGrip = Grip; }

	void SetBarrel(FWeaponBarrel & WeaponBarrel) { Barrel = WeaponBarrel; }
	void SetBody(FWeaponBody & WeaponBody) { Body = WeaponBody; }
	void SetGrip(FWeaponGrip & WeaponGrip) { Grip = WeaponGrip; }
};

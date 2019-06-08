// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/ItemBlueprint.h"

#include "CoreMinimal.h"
#include "AmmunitionBlueprint.generated.h"

// Forward declarations.
class UTexture2D;


/**
 * Used by the LootAmmoBuilder class to instantiate AmmunitionPile objects.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UAmmunitionBlueprint : public UItemBlueprint
{
	GENERATED_BODY()
	
private:
	/** Name of the ammunition pile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Blueprint", Meta = (AllowPrivateAccess = "true"))
	FText Name;

	/** Icon of the ammunition pile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Blueprint", Meta = (AllowPrivateAccess = "true"))
	UTexture2D* Icon;

	/** Type of weapon the ammunition pile will be used for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Blueprint", Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponTypeAmmo;

	/**
	 * Amount of ammunition units the ammunition pile will contain, defined by an interval.
	 * Actual value will be randomly chosen from this interval.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Blueprint", Meta = (AllowPrivateAccess = "true"))
	FItemAttribute_IntervalInt CurrentAmmoUnitsCount;

public:
	FText GetItemName() const { return Name; }
	UTexture2D* GetItemIcon() const { return Icon; }
	EWeaponType GetAmmoType() const { return WeaponTypeAmmo; }
	void GetAmmoCountRandomized(FItemAttribute_Float & AmmoAttr) const;
};

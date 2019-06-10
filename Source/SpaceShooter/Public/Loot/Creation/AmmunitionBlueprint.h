// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/ItemBlueprint.h"

#include "CoreMinimal.h"
#include "SubclassOf.h"
#include "AmmunitionBlueprint.generated.h"

// Forward declarations.
class UItemRarity;


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

	/** Rarity of the ammunition pile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Blueprint", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemRarity> Rarity;

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
	TSubclassOf<UItemRarity> GetAmmoRarity() const { return Rarity; }
	void GetAmmoCountRandomized(FItemAttribute_Float & AmmoAttr) const;
};

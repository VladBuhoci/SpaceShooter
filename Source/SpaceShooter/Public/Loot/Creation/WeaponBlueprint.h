// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/ItemRarity.h"

#include "CoreMinimal.h"
#include "SubclassOf.h"
#include "WeaponBlueprint.generated.h"


/**
 * Used by the LootWeaponBuilder class to instantiate Weapon objects.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UWeaponBlueprint : public UItemBlueprint
{
	GENERATED_BODY()
	
private:
	/** Rarity of the weapon to be spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemRarity> Rarity;

	/** Type of the weapon to be spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	EWeaponType Type;

public:
	TSubclassOf<UItemRarity> GetRarity() const { return Rarity; }
	EWeaponType GetType() const { return Type; }

	void SetRarity(TSubclassOf<UItemRarity> ItemRarity) { Rarity = ItemRarity; }
	void SetType(EWeaponType WeaponType) { Type = WeaponType; }
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SubclassOf.h"
#include "WeaponPool.generated.h"

// Forward declarations.
class UItemRarity;
class UWeaponPartsPool;

struct FWeaponBarrel;
struct FWeaponBody;
struct FWeaponGrip;


USTRUCT(BlueprintType)
struct FWeaponRarityPool
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool | Rarity Group")
	TSubclassOf<UWeaponPartsPool> BlasterPartsPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool | Rarity Group")
	TSubclassOf<UWeaponPartsPool> CannonPartsPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool | Rarity Group")
	TSubclassOf<UWeaponPartsPool> VolleyPartsPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool | Rarity Group")
	TSubclassOf<UWeaponPartsPool> LauncherPartsPool;
};

/**
 * Used as a global pool for all weapon parts, organized by rarities and types.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UWeaponPool : public UObject
{
	GENERATED_BODY()
	
protected:
	/** Each complete set of weapon categories is mapped to a rarity type. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool")
	TMap<TSubclassOf<UItemRarity>, FWeaponRarityPool> Pools;

public:
	/**
	 * Provides a random barrel, body and grip from a pool, given a weapon rarity and type.
	 * @returns true if all 3 parts were found, false otherwise.
	 */
	bool GetWeaponParts(TSubclassOf<UItemRarity> Rarity, EWeaponType Type, FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip) const;

private:
	UWeaponPartsPool* GetPartsPool(TSubclassOf<UItemRarity> Rarity, EWeaponType Type) const;
};

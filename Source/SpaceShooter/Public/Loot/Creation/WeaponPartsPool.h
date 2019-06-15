// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"

#include "Loot/WorldAwareObject.h"
#include "SubclassOf.h"
#include "WeaponPartsPool.generated.h"

// Forward declarations.
class UItemRarity;
class AProjectile;

class UStaticMesh;
class UMaterialInterface;
class UParticleSystem;
class USoundBase;


USTRUCT(BlueprintType)
struct SPACESHOOTER_API FWeaponBarrel
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Barrels")
	FText WeaponNameBase;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Barrels")
	UStaticMesh* BarrelMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Barrels")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Barrels")
	USoundBase* FiringSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Barrels")
	UParticleSystem* FiringEffect;

	/** Pairs of weapon attributes and the values that will modify them in the final weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool")
	TMap<EWeaponAttribute, float> AttributeModifierMap;
};

USTRUCT(BlueprintType)
struct SPACESHOOTER_API FWeaponBody
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Bodies")
	FText WeaponNamePrefix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Bodies")
	UStaticMesh* BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Bodies")
	UMaterialInterface* WeaponMaterial;

	/** Pairs of weapon attributes and the values that will modify them in the final weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool")
	TMap<EWeaponAttribute, float> AttributeModifierMap;
};

USTRUCT(BlueprintType)
struct SPACESHOOTER_API FWeaponGrip
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Grips")
	FText WeaponNamePrefix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool | Grips")
	UStaticMesh* GripMesh;

	/** Pairs of weapon attributes and the values that will modify them in the final weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Pool")
	TMap<EWeaponAttribute, float> AttributeModifierMap;
};

/**
 * Contains collections of different parts from which the LootWeaponBuilder takes some and constructs a weapon.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UWeaponPartsPool : public UWorldAwareObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool")
	TArray<FWeaponBarrel> Barrels;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool")
	TArray<FWeaponBody> Bodies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot | Weapon Parts Pool")
	TArray<FWeaponGrip> Grips;

public:
	/**
	 * Provides a random barrel, body and grip from this pool.
	 * @returns true if all 3 parts were found, false otherwise.
	 */
	bool GetWeaponParts(FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip) const;

private:
	bool RetrieveRandomBarrel(FWeaponBarrel & Barrel) const;
	bool RetrieveRandomBody(FWeaponBody & Body) const;
	bool RetrieveRandomGrip(FWeaponGrip & Grip) const;
};

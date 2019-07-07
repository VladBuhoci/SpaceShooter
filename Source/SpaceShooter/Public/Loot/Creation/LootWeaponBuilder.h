// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"
#include "Loot/Creation/LootItemBuilder.h"

#include "CoreMinimal.h"
#include "LootWeaponBuilder.generated.h"

// Forward declarations.
class AWeapon;
class UWeaponBlueprint;
class UItemRarity;
class USpaceGameInstance;

struct FWeaponAttributes;
struct FWeaponBarrel;
struct FWeaponBody;
struct FWeaponGrip;


/**
 * Weapon-type loot item builder.
 */
UCLASS()
class SPACESHOOTER_API ULootWeaponBuilder : public ULootItemBuilder
{
	GENERATED_BODY()
	
private:
	USpaceGameInstance* SpaceGameInstance;

public:
	virtual AItem* Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform = FTransform::Identity) override;
	virtual AItem* Build(UItemBlueprint* ItemToBuildBlueprint, const FTransform & SpawnedItemTransform = FTransform::Identity) override;

private:
	void SetUpWeapon(AWeapon* Weapon, FWeaponAttributes & InitValues, EWeaponType ItemType, TSubclassOf<UItemRarity> ItemRarity,
		FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip);
	
	TMap<EWeaponAttribute, float> GetCombinedAttributeModifierMap(FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip);

	void CombineTwoModifierMaps(TMap<EWeaponAttribute, float> & Map1, TMap<EWeaponAttribute, float> & Map2);

	void ModifyWeaponAttributesWithModifierMap(FWeaponAttributes & Attributes, TMap<EWeaponAttribute, float> & Modifiers);

	USpaceGameInstance* GetSpaceGameInstance();
};

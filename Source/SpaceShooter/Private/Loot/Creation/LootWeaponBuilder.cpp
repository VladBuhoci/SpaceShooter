// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/LootWeaponBuilder.h"
#include "Loot/Creation/ItemBlueprint.h"
#include "Loot/Creation/WeaponBlueprint.h"
#include "Loot/Creation/PreciseWeaponBlueprint.h"
#include "Loot/Creation/WeaponPool.h"
#include "Loot/Creation/WeaponPartsPool.h"
#include "Loot/Items/Item.h"
#include "Loot/Items/Weapon.h"
#include "GameModes/SpaceGameMode.h"
#include "Globals/SpaceGameInstance.h"

#include "Materials/MaterialInterface.h"

#include "Particles/ParticleSystem.h"

#include "Sound/SoundBase.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"


#define WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(AttribStruct, Map, Attrib) \
	if (Map.Contains(EWeaponAttribute::Attrib)) \
	{ \
		AttribStruct.Attrib += AttribStruct.Attrib * Map[EWeaponAttribute::Attrib] / 100.0f; \
	}

#define WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_UNIT_VALUE(AttribStruct, Map, Attrib) \
	if (Map.Contains(EWeaponAttribute::Attrib)) \
	{ \
		AttribStruct.Attrib += Map[EWeaponAttribute::Attrib]; \
	}


AItem* ULootWeaponBuilder::Build(TSubclassOf<UItemBlueprint> ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	if (!ItemToBuildBlueprint)
		return nullptr;

	UWeaponBlueprint* WeaponBP = NewObject<UWeaponBlueprint>(this, ItemToBuildBlueprint);

	return Build(WeaponBP, SpawnedItemTransform);
}

AItem* ULootWeaponBuilder::Build(UItemBlueprint* ItemToBuildBlueprint, const FTransform & SpawnedItemTransform)
{
	if (!ItemToBuildBlueprint)
		return nullptr;

	AWeapon* SpawnedWeapon = nullptr;
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));
		
		if (SpaceGameMode)
		{
			FWeaponBarrel Barrel;
			FWeaponBody   Body;
			FWeaponGrip   Grip;

			EWeaponType Type = EWeaponType::Unknown;
			TSubclassOf<UItemRarity> Rarity;

			// Obtain the components to set up the weapon.
			if (ItemToBuildBlueprint->GetClass()->IsChildOf(UPreciseWeaponBlueprint::StaticClass()))
			{
				UPreciseWeaponBlueprint* WeaponBP = Cast<UPreciseWeaponBlueprint>(ItemToBuildBlueprint);

				if (WeaponBP)
				{
					WeaponBP->GetBarrel(Barrel);
					WeaponBP->GetBody(Body);
					WeaponBP->GetGrip(Grip);

					Type = WeaponBP->GetType();
					Rarity = WeaponBP->GetRarity();
				}
			}
			else
			{
				UWeaponBlueprint* WeaponBP = Cast<UWeaponBlueprint>(ItemToBuildBlueprint);

				if (WeaponBP)
				{
					SpaceGameMode->GetGlobalWeaponPool()->GetWeaponParts(WeaponBP->GetRarity(), WeaponBP->GetType(), Barrel, Body, Grip);

					Type = WeaponBP->GetType();
					Rarity = WeaponBP->GetRarity();
				}
			}

			// Create the bare-bones weapon.
			SpawnedWeapon = WorldPtr->SpawnActor<AWeapon>(AWeapon::StaticClass(), SpawnedItemTransform);

			if (SpawnedWeapon)
			{
				FWeaponAttributes AttrValues;

				SpaceGameMode->GetGlobalWeaponPool()->GetWeaponTemplate(Type, AttrValues);

				// Fill up the variables.
				SetUpWeapon(SpawnedWeapon, AttrValues, Type, Rarity, Barrel, Body, Grip);

				// Save a persistent copy of this weapon's components in the Space Game Instance object.
				GetSpaceGameInstance()->CreateWeaponDescriptor(Type, Rarity, Barrel, Body, Grip, SpawnedWeapon);
			}
		}
	}

	return SpawnedWeapon;
}

void ULootWeaponBuilder::SetUpWeapon(AWeapon* Weapon, FWeaponAttributes & InitValues, EWeaponType ItemType, TSubclassOf<UItemRarity> ItemRarity,
	FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip)
{
	FText        ItemName = FText::Format(FText::FromString("{0}-{1} {2}"), Grip.WeaponNamePrefix, Body.WeaponNamePrefix, Barrel.WeaponNameBase);
	UTexture2D*  Icon     = InitValues.Icon;
	UItemRarity* Rarity   = NewObject<UItemRarity>(this, ItemRarity);
	EWeaponType  Type     = ItemType;
	// ...
	UStaticMesh*        BarrelMesh   = Barrel.BarrelMesh;
	UStaticMesh*        BodyMesh     = Body.BodyMesh;
	UStaticMesh*        GripMesh     = Grip.GripMesh;
	UMaterialInterface* MeshMaterial = Body.WeaponMaterial;
	// ...
	TSubclassOf<AProjectile> ProjectileClass = Barrel.ProjectileClass;
	UParticleSystem*         FiringEffect    = Barrel.FiringEffect;
	USoundBase*              FiringSound     = Barrel.FiringSound;
	// ...
	TMap<EWeaponAttribute, float> AttributeModifierMap = GetCombinedAttributeModifierMap(Barrel, Body, Grip);
	
	ModifyWeaponAttributesWithModifierMap(InitValues, AttributeModifierMap);

	Weapon->SetItemName(ItemName);
	Weapon->SetItemIcon(Icon);
	Weapon->SetItemRarity(Rarity);
	Weapon->SetType(Type);
	// ...
	Weapon->SetBarrelMesh(BarrelMesh);
	Weapon->SetBodyMesh(BodyMesh);
	Weapon->SetGripMesh(GripMesh);
	Weapon->SetMaterial(MeshMaterial);
	// ...
	Weapon->SetProjectileClass(ProjectileClass);
	Weapon->SetFiringEffect(FiringEffect);
	Weapon->SetFiringSound(FiringSound);
	// ...
	Weapon->SetNumericAttributes(InitValues);
}

TMap<EWeaponAttribute, float> ULootWeaponBuilder::GetCombinedAttributeModifierMap(FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip)
{
	TMap<EWeaponAttribute, float> FinalMap = Barrel.AttributeModifierMap;

	CombineTwoModifierMaps(FinalMap, Body.AttributeModifierMap);
	CombineTwoModifierMaps(FinalMap, Grip.AttributeModifierMap);

	return FinalMap;
}

void ULootWeaponBuilder::CombineTwoModifierMaps(TMap<EWeaponAttribute, float> & Map1, TMap<EWeaponAttribute, float> & Map2)
{
	TArray<EWeaponAttribute> Keys;
	
	Map2.GetKeys(Keys);

	for (EWeaponAttribute ModifierKey : Keys)
	{
		if (Map1.Contains(ModifierKey))
		{
			Map1.Add(ModifierKey, Map1[ModifierKey] + Map2[ModifierKey]);
		}
		else
		{
			Map1.Add(ModifierKey, Map2[ModifierKey]);
		}
	}
}

void ULootWeaponBuilder::ModifyWeaponAttributesWithModifierMap(FWeaponAttributes & Attributes, TMap<EWeaponAttribute, float> & Modifiers)
{
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, Damage)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_UNIT_VALUE(Attributes, Modifiers, ProjectilesPerShot)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, SpreadAngle)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, Accuracy)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, AccuracyRecoveryRate)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, AccuracyRecoveryDelay)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, FireRate)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, Recoil)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, HeatProducedPerShot)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_PERCENTAGE_VALUE(Attributes, Modifiers, CoolingRate)
	WEAPON_ATTRIBUTE_MODIFIER_MAP_HELPER_UNIT_VALUE(Attributes, Modifiers, AmmoPerShot)
}

USpaceGameInstance* ULootWeaponBuilder::GetSpaceGameInstance()
{
	if (!SpaceGameInstance)
	{
		SpaceGameInstance = Cast<USpaceGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}

	return SpaceGameInstance;
}

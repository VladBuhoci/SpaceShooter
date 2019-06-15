// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/WeaponPool.h"
#include "Loot/Creation/WeaponPartsPool.h"
#include "Loot/ItemRarity.h"


bool UWeaponPool::GetWeaponParts(TSubclassOf<UItemRarity> Rarity, EWeaponType Type, FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip) const
{
	if (!Rarity)
		return false;

	UWeaponPartsPool* Pool = GetPartsPool(Rarity, Type);

	if (Pool == nullptr)
		return false;

	return Pool->GetWeaponParts(Barrel, Body, Grip);
}

bool UWeaponPool::GetWeaponTemplate(EWeaponType Type, FWeaponAttributes & ValuesTemplate)
{
	if (WeaponTypeTemplates.Num() == 0 || !WeaponTypeTemplates.Contains(Type))
		return false;

	ValuesTemplate = WeaponTypeTemplates[Type];

	return true;
}

UWeaponPartsPool* UWeaponPool::GetPartsPool(TSubclassOf<UItemRarity> Rarity, EWeaponType Type) const
{
	if (Pools.Num() == 0 || !Pools.Contains(Rarity))
		return nullptr;

	UE_LOG(LogTemp, Log, TEXT("Weapon rarity: %s"), *Rarity->GetFullName());
	UE_LOG(LogTemp, Log, TEXT("Checking the global weapon pool for a RarityPool group..."));
	
	FWeaponRarityPool RarityPool = Pools[Rarity];
	TSubclassOf<UWeaponPartsPool> WeaponPartsPoolClass = NULL;
	UWeaponPartsPool* WeaponPartsPoolInstance = nullptr;
	
	UE_LOG(LogTemp, Log, TEXT("Pooling done."));

	switch (Type)
	{
	case EWeaponType::Blaster:  WeaponPartsPoolClass = RarityPool.BlasterPartsPool;  break;
	case EWeaponType::Cannon:   WeaponPartsPoolClass = RarityPool.CannonPartsPool;   break;
	case EWeaponType::Volley:   WeaponPartsPoolClass = RarityPool.VolleyPartsPool;   break;
	case EWeaponType::Launcher: WeaponPartsPoolClass = RarityPool.LauncherPartsPool; break;

	default: break;
	}

	UE_LOG(LogTemp, Log, TEXT("WeaponPartsPoolClass: %s"), WeaponPartsPoolClass ? *WeaponPartsPoolClass->GetFullName() : TEXT("None found"));
	
	if (WeaponPartsPoolClass)
	{
		WeaponPartsPoolInstance = NewObject<UWeaponPartsPool>((UObject*) this, WeaponPartsPoolClass);
	}

	return WeaponPartsPoolInstance;
}

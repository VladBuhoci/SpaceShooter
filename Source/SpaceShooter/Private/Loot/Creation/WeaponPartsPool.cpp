// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/WeaponPartsPool.h"


bool UWeaponPartsPool::GetWeaponParts(FWeaponBarrel & Barrel, FWeaponBody & Body, FWeaponGrip & Grip) const
{
	return RetrieveRandomBarrel(Barrel) && RetrieveRandomBody(Body) && RetrieveRandomGrip(Grip);
}

bool UWeaponPartsPool::RetrieveRandomBarrel(FWeaponBarrel & Barrel) const
{
	if (Barrels.Num() == 0)
		return false;

	int32 RandIndex = FMath::RandRange(0, Barrels.Num() - 1);

	if (Barrels.IsValidIndex(RandIndex))
	{
		Barrel = Barrels[RandIndex];
		return true;
	}
	
	return false;
}

bool UWeaponPartsPool::RetrieveRandomBody(FWeaponBody & Body) const
{
	if (Bodies.Num() == 0)
		return false;

	int32 RandIndex = FMath::RandRange(0, Bodies.Num() - 1);

	if (Bodies.IsValidIndex(RandIndex))
	{
		Body = Bodies[RandIndex];
		return true;
	}

	return false;
}

bool UWeaponPartsPool::RetrieveRandomGrip(FWeaponGrip & Grip) const
{
	if (Grips.Num() == 0)
		return false;

	int32 RandIndex = FMath::RandRange(0, Grips.Num() - 1);

	if (Grips.IsValidIndex(RandIndex))
	{
		Grip = Grips[RandIndex];
		return true;
	}

	return false;
}

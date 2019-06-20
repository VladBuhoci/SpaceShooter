// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpaceStructs.generated.h"


/**
 * Describes the ammo stock for one particular weapon type.
 */
USTRUCT(BlueprintType)
struct FAmmunitionStock
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons")
	bool bEndlessAmmo = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (EditCondition = "!bEndlessAmmo"))
	int32 CurrentAmmoQuantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (EditCondition = "!bEndlessAmmo"))
	int32 MaxAmmoQuantity;

	FAmmunitionStock()				                   : FAmmunitionStock(0)										{};
	FAmmunitionStock(bool bInfiniteAmmo)			   : bEndlessAmmo(bInfiniteAmmo)								{};
	FAmmunitionStock(int32 CurrMaxAmmo)                : FAmmunitionStock(CurrMaxAmmo, CurrMaxAmmo)					{};
	FAmmunitionStock(int32 CurrentAmmo, int32 MaxAmmo) : CurrentAmmoQuantity(CurrentAmmo), MaxAmmoQuantity(MaxAmmo)	{};
};

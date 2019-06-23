// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpaceStructs.generated.h"

// Forward declarations.
class UTexture2D;


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

/**
 * Used as templates to easily configure common attributes (like Damage, Accuracy etc.) once only.
 */
USTRUCT(BlueprintType)
struct FItemAttributeDescription
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	FText Name;

	FItemAttributeDescription()
		: Icon(nullptr), Name(FText::FromString("-"))
	{}
};

/**
 * Contains the usual game stats (total kills, shots fired, shots hit (defining player's average accuracy) etc.)
 */
USTRUCT(BlueprintType)
struct FLevelOverallStats
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD")
	int32 TotalKills;
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpaceEnums.generated.h"


/**
 * The "side" which a spacecraft belongs to.
 */
UENUM(BlueprintType)
enum class ESpacecraftFaction : uint8
{
	/** Not belonging to any side. */
	Unspecified,

	/** The human race. */
	Human,

	/** The clone race. */
	Clone,

	/** Do not use this directly. Introduced for C++ only. */
	Both = Human | Clone
};

/**
 * Weapon categories.
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unknown,

	/** Normal, single projectile firing weapon. */
	Blaster,

	/** Fast, single projectile firing weapon. */
	Cannon,

	/** Multi-projectile firing weapon. */
	Volley,

	/** Missile firing weapon. */
	Launcher
};

/**
 * Weapon attributes, as enumerated constants, paired with modifying values as percentages/units in the weapon
 *	building process can (and will) influence the final result - the weapon built using random parts from the global pool.
 */
UENUM(BlueprintType)
enum class EWeaponAttribute : uint8
{
	Unknown,

	Damage,
	ProjectilesPerShot,
	SpreadAngle,
	Accuracy,
	AccuracyRecoveryRate,
	AccuracyRecoveryDelay,
	FireRate,
	Recoil,
	HeatProducedPerShot,
	CoolingRate,
	AmmoPerShot
};

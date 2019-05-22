// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SpaceEnums.generated.h"


/**
 * The "side" which a spacecraft belongs to.
 */
UENUM(BlueprintType)
enum class ESpacecraftFaction : uint8
{
	/** Not belonging to any side. */
	Unspecified = 0,

	/** The human race. */
	Human       = 1,

	/** The clone race. */
	Clone       = 2,

	/** Do not use this directly. Introduced for C++ only. */
	Both        = Human | Clone
};

/**
 * Weapon categories.
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unknown,

	/** Normal, (usually) single projectile firing weapon. */
	Blaster,

	/** Faster firing weapon. */
	Cannon,

	/** Multi-projectile firing weapon. */
	Shotgun,

	/** Missile firing weapon. */
	Launcher
};

/**
 * 
 */
UCLASS()
class SPACESHOOTER_API USpaceEnums : public UObject
{
	GENERATED_BODY()

public:
	USpaceEnums(const FObjectInitializer & ObjectInitializer);
};

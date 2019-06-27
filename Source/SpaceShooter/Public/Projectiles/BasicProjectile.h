// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Projectile.h"
#include "BasicProjectile.generated.h"

// Forward declarations.
class ASpacecraftPawn;


/**
 * Normally behaving projectiles.
 */
UCLASS()
class SPACESHOOTER_API ABasicProjectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	/** Sets default values. */
	ABasicProjectile();

protected:
	/** Applies damage to a certain actor or actors. */
	virtual void ApplyDamage(ASpacecraftPawn* HitPawn);
};

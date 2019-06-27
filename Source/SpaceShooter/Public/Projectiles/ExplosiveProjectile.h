// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Projectiles/Projectile.h"
#include "ExplosiveProjectile.generated.h"

// Forward declarations.
class ASpacecraftPawn;


/**
 * Hitting an enemy causes these projectiles to explode and cause radial damage to nearby baddies.
 */
UCLASS()
class SPACESHOOTER_API AExplosiveProjectile : public AProjectile
{
	GENERATED_BODY()
	
	/** Minimum damage all actors in the radius will take. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	int32 MinimumDamage;

	/** Maximum damage explosion (inner) area radius. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	float MaxDamageRadius;

	/** Minimum damage explosion (outer) area radius. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	float MinDamageRadius;

	/** Falloff exponent of damage from MaxDamageRadius to MinDamageRadius. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	float DamageFalloff;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	ASpacecraftPawn* DamageCauser;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	AController* EventInstigator;

public:
	/** Sets default values. */
	AExplosiveProjectile();

protected:
	/** Applies damage to a certain actor or actors. */
	virtual void ApplyDamage(ASpacecraftPawn* HitPawn);
};

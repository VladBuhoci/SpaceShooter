// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

// Forward declarations.
class AProjectile;

/**
 * Base class of all spaceship weapons.
 */
UCLASS()
class SPACESHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
private:
	/** Mesh component of this weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	/** Class of the projectile to spawn when shooting. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectile> ProjectileClass;

	/** Amount of projectiles to shoot per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float FireRate;

	/** Amount of time which has passed since the last moment the weapon has been fired. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float TimePassedSinceLastShot;

public:
	/** Sets default values for this actor's properties. */
	AWeapon();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FireWeapon(EProjectileOwnerType OwnerType);

private:
	void ResetTimeSinceLastWeaponUsage();
	bool IsAllowedToFireWeapon();
};

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectile> ProjectileClass;

	/** Amount of damage one projectile can cause. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float Damage;

	/**
	 * Determines the triangular area in which projectiles can be fired.
	 * Higher values mean more space can be covered but it becomes harder to hit a specific target.
	 * The spread angle is the angle between the center of the cone and any of its edges.
	 * It is half the angle of the cone's aperture, thus the aperture's angle will be SpreadAngle * 2.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true", ClampMin = "5", UIMin = "5", ClampMax = "45", UIMax = "45"))
	float SpreadAngle;
	
	/** Affects the precision of the projectiles fired by this weapon: higher value = higher chance to hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float Accuracy;

	/** This is the actual accuracy used when firing the weapon. For details, see: Accuracy. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float CurrentAccuracy;

	/** Amount of accuracy units to recover per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float AccuracyRecoveryRate;

	/** Time (in seconds) to pass before the accuracy begins to return to its base value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float AccuracyRecoveryDelay;

	/** Used to control the timer which triggers the process of restoring accuracy to its initial value. */
	FTimerHandle CountToBeginAccuracyRecoveryTimer;

	/** Amount of projectiles to shoot per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float FireRate;

	/** Recoil is applied to the current accuracy whenever this weapon shoots something, making it less precise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float Recoil;

	/** Amount of time which has passed since the last moment the weapon has been fired, in seconds. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float TimePassedSinceLastShot;

	/** Particle system which is spawned whenever this weapon shoots projectiles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* WeaponFiringParticleEffect;

	/** Sound played whenever this weapon shoots projectiles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	USoundBase* WeaponFiringSound;

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
	virtual void FireWeapon(ASpacecraftPawn* ProjectileOwner);

private:
	void ResetTimeSinceLastWeaponUsage();
	bool IsAllowedToFireWeapon();
	void PlayWeaponFiringEffects();
	void ApplyRecoil();
	void RecoverAccuracyByOneWholeRate();
};

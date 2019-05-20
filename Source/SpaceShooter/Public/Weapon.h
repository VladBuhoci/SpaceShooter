// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

// Forward declarations.
class AProjectile;


/**
 * Base class of all spaceship weapons.
 */
UCLASS()
class SPACESHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
private:
	/** Mesh component of this weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	/** Type of this weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	EWeaponType Type;

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

	/** True if Accuracy is currently returning to its initial value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	bool bIsAccuracyBeingRestored;

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

	/**
	 * A value between 0% and 100% that once reaches its peak, it enters an overheated state
	 *		and the weapon cannot be used again until it has completely cooled down.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float CurrentHeat;

	/** Amount of heat produced every single time the weapon is used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float HeatProducedPerShot;

	/** Heat points "cooled down" (removed) per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float CoolingRate;

	/** If true, the weapon can't be fired until CurrentHeat reaches 0%. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	bool bIsOverheated;

	/** Ammunition units consumed per shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	int32 AmmoPerShot;

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
	virtual void FireWeapon(ASpacecraftPawn* ProjectileOwner, int32 & AmmoToUse);

public:
	void SetVisibility(bool CurrentState);

private:
	void ResetTimeSinceLastWeaponUsage();
	bool IsAllowedToFireWeapon();
	void PlayWeaponFiringEffects();
	void ApplyRecoil();

	void CheckAccuracyStatus(float DeltaTime);
	void ScheduleAccuracyRecoveryProcess();
	void BeginAccuracyRecoveryProcess();
	void StopAccuracyRecoveryProcess();
	void RecoverAccuracy(float DeltaTime);

	void CheckHeatState(float DeltaTime);
	void ProduceHeat();
	void EnterOverheatedState();
	void ExitOverheatedState();
	void CoolDown(float DeltaTime);

	bool HasEnoughAmmoForOneShot();
	void ConsumeAmmoForOneShot(int32 & AmmoToUse);

public:
	/** Returns the current percentage of total heat produced. */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	float GetCurrentHeatLevel() const { return CurrentHeat; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool IsOverheated() const { return bIsOverheated; }


	/**********************************
				GETTERS
	**********************************/

public:
	/** Returns the type of this weapon. */
	UFUNCTION(BlueprintPure, Category = "Weapon")
	EWeaponType GetType() const { return Type; }
};

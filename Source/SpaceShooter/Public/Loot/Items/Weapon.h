// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Items/Item.h"

#include "CoreMinimal.h"
#include "Weapon.generated.h"

// Forward declarations.
class AProjectile;

class UMaterialInterface;


/**
 * Contains every public weapon attribute.
 */
USTRUCT(BlueprintType)
struct FWeaponAttributes
{
	GENERATED_USTRUCT_BODY()

	/** Amount of damage one projectile can cause. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 Damage;

	/** Determines the amount of projectiles spawned per shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 ProjectilesPerShot = 1;

	/**
	 * Determines the triangular area in which projectiles can be fired.
	 * Higher values mean more space can be covered but it becomes harder to hit a specific target.
	 * The spread angle is the angle between the center of the cone and any of its edges.
	 * It is half the angle of the cone's aperture, thus the aperture's angle will be SpreadAngle * 2.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (ClampMin = "5", UIMin = "5", ClampMax = "45", UIMax = "45"))
	float SpreadAngle;

	/** Affects the precision of the projectiles fired by this weapon: higher value = higher chance to hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", Meta = (ClampMin = "0", UIMin = "0", ClampMax = "100", UIMax = "100"))
	float Accuracy;

	/** Amount of accuracy units to recover per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AccuracyRecoveryRate;

	/** Time (in seconds) to pass before the accuracy begins to return to its base value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float AccuracyRecoveryDelay;

	/** Amount of projectiles to shoot per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float FireRate;

	/** Recoil is applied to the current accuracy whenever this weapon shoots something, making it less precise. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Recoil;

	/** Amount of heat produced every single time the weapon is used. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float HeatProducedPerShot;

	/** Heat points "cooled down" (removed) per second. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float CoolingRate;

	/** Ammunition units consumed per shot. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	int32 AmmoPerShot = 1;
};

/**
 * Base class of all spaceship weapons.
 */
UCLASS()
class SPACESHOOTER_API AWeapon : public AItem
{
	GENERATED_BODY()
	
protected:
	/** Mesh component of this weapon. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMeshComponent* MeshComponent;

	/** Type of this weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	EWeaponType Type;

	/** Class of the projectile to spawn when shooting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AProjectile> ProjectileClass;

	/** Particle system which is spawned whenever this weapon shoots projectiles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* WeaponFiringParticleEffect;

	/** Sound played whenever this weapon shoots projectiles. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* WeaponFiringSound;

	/** Weapon public numeric attributes. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FWeaponAttributes WeaponAttributes;

private:
	/** This is the actual accuracy used when firing the weapon. For details, see: Accuracy. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float CurrentAccuracy;

	/** True if Accuracy is currently returning to its initial value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	bool bIsAccuracyBeingRestored;

	/** Used to control the timer which triggers the process of restoring accuracy to its initial value. */
	FTimerHandle CountToBeginAccuracyRecoveryTimer;

	/** Amount of time which has passed since the last moment the weapon has been fired, in seconds. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float TimePassedSinceLastShot;

	/**
	 * A value between 0% and 100% that once reaches its peak, it enters an overheated state
	 *		and the weapon cannot be used again until it has completely cooled down.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	float CurrentHeat;

	/** If true, the weapon can't be fired until CurrentHeat reaches 0%. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	bool bIsOverheated;

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

	virtual void ProvideAttributes(TArray<FItemAttribute_Float> & AttributesArrayToSupply) override;

public:
	void SetVisibility(bool CurrentState);

private:
	float ComputeInaccuracyFactor(float ProjectileSpreadAngle);
	AProjectile* SpawnProjectile(UWorld* World, FVector & ProjectileLocation, FRotator & ProjectileRotationBase,
		ASpacecraftPawn* ProjectileOwner, float InitialRotationDeviation, float InaccuracyFactorMin, float InaccuracyFactorMax);
	
	void DoPostFiringTasks(int32 & AmmoToUse);
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

	bool HasEnoughAmmoForOneShot(const int32 & AmmoToUse) const;
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


	/**********************************
				 SETTERS
	**********************************/

public:
	void SetType(EWeaponType WeaponType) { this->Type = WeaponType; }

	void SetBarrelMesh(UStaticMesh* Mesh);
	void SetBodyMesh(UStaticMesh* Mesh);
	void SetGripMesh(UStaticMesh* Mesh);
	void SetMaterial(UMaterialInterface* Mat);
	
	void SetProjectileClass(TSubclassOf<AProjectile> Clazz) { this->ProjectileClass = Clazz; }
	void SetFiringEffect(UParticleSystem* Effect) { this->WeaponFiringParticleEffect = Effect; }
	void SetFiringSound(USoundBase* Sound) { this->WeaponFiringSound = Sound; }

	void SetNumericAttributes(FWeaponAttributes & Attributes);
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "SpaceEnums.h"

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpacecraftPawn.generated.h"

// Forward declarations.
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UParticleSystemComponent;

class AProjectile;
class AWeapon;


/**
 * Base class of all spacecrafts (player or NPCs).
 */
UCLASS()
class SPACESHOOTER_API ASpacecraftPawn : public APawn
{
	GENERATED_BODY()

protected:
	//////////////////////////////////////////////////////////////////////////
	// Class Components.
	//////////////////////////////////////////////////////////////////////////

	/** The mesh of the spacecraft. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UStaticMeshComponent* SpacecraftMeshComponent;

	/** Movement component governing the movement of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UFloatingPawnMovement* SpacecraftMovementComponent;

	/** Particle system which is activated when the spacecraft is moving forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UParticleSystemComponent* BackSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is moving backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UParticleSystemComponent* FrontSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is rotating clockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UParticleSystemComponent* LeftSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is rotating counterclockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components")
	UParticleSystemComponent* RightSideThrusterParticleEmitter;


	//////////////////////////////////////////////////////////////////////////
	// Movement.
	//////////////////////////////////////////////////////////////////////////

	/** True if the spacecraft is flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	bool bIsMovingForward;

	/** True if the spacecraft is flying backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	bool bIsMovingBackward;

	/**
	 * True if the spacecraft is in turbo mode, significantly increasing the movement speed of the spacecraft.
	 * Only available for forward flight.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	bool bIsTurboModeActive;

	/** Maximum speed of the spacecraft while turbo mode is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	float MoveForwardMaxSpeed;

	/** Maximum speed of the spacecraft while flying normally. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	float MoveForwardMaxTurboSpeed;

	/** Current maximum speed of the spacecraft when flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	float MoveForwardSpeed;

	/** Current maximum speed of the spacecraft when flying backward. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement")
	float MoveBackwardSpeed;

	/** A higher value means the spacecraft turns left or right quicker. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true", ClampMin = "8", UIMin = "8", ClampMax = "20", UIMax = "20"))
	float SpacecraftTurnSpeed;


	//////////////////////////////////////////////////////////////////////////
	// Effects.
	//////////////////////////////////////////////////////////////////////////

	/** Particle system template for normal forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects")
	UParticleSystem* BacksideNormalThrusterParticleSystem;

	/** Particle system template for turbo forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects")
	UParticleSystem* BacksideTurboThrusterParticleSystem;

	/** Particle system which is spawned when this spacecraft is destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects")
	UParticleSystem* DestroyParticleEffect;

	/** Sound played when this spacecraft is destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects")
	USoundBase* DestroySound;


	//////////////////////////////////////////////////////////////////////////
	// Survivability.
	//////////////////////////////////////////////////////////////////////////

	/** Maximum amount of damage this spacecraft can take before it gets destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	int32 MaxHitPoints;

	/** Current amount of damage this spacecraft can take before it gets destroyed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	int32 CurrentHitPoints;

	/** The energy shield's maximum capacity. Once it is zero it can no longer protect this spacecraft. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	int32 MaxShieldPoints;

	/** Current amount of energy shield points. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	int32 CurrentShieldPoints;

	/**
	 * Amount of damage this shield absorbs (percentage of total damage received), losing an equivalent 
	 *		amount of energy points in the process.
	 * The remaining damage (that did not get absorbed) will be inflicted onto the spacecraft itself.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	int32 ShieldAbsorptionRate;

	/** The rate at which this ship's shield recharges itself to full capacity, measured in points per second. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	float ShieldRechargeRate;

	/** Amount of time which has passed since the last moment the shield has regained 1 point of energy, in seconds. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	float ShieldRechargeTimePassedSinceLastPointRecharged;

	/**
	 * Once damaged, the shield will attempt to recharge itself, but only after a short amount of time
	 *		has passed since last hit.
	 * Receiving damage "resets" the timer paired with this delay.
	 * The delay is measured in seconds.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	float ShieldRechargeDelay;

	/** True if the shield is currently recharging itself. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability")
	bool bIsShieldRecharging;

	/**
	 * Works along with ShieldRechargeDelay to make sure the shield begins to recharge when it should do so.
	 * Getting hit will clear this timer handle and schedule another recharge attempt after the specified delay has passed.
	 */
	FTimerHandle ShieldRechargeTimerHandle;


	//////////////////////////////////////////////////////////////////////////
	// Weapons.
	//////////////////////////////////////////////////////////////////////////

	/** Primary weapon class. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons")
	TSubclassOf<AWeapon> PrimaryWeaponClass;
	
	/** Primary weapon reference. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons")
	AWeapon* PrimaryWeapon;

	/** If true, all attached primary weapons will be fired together. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons")
	bool bIsFiringPrimaryWeapons;


	//////////////////////////////////////////////////////////////////////////
	// Miscellaneous.
	//////////////////////////////////////////////////////////////////////////

	/** The "side" this spacecraft belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Misc")
	ESpacecraftFaction Faction;

public:
	/** Sets default values. */
	ASpacecraftPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	virtual void InitializeAttributes();


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

public:
	void MaintainForwardMovementSetup();
	void MaintainBackwardMovementSetup();

	virtual void MoveForward(float Value);
	virtual void MoveBackward(float Value);
	virtual void RotateSpacecraft(FRotator rotator);
	virtual void ActivateTurboMode();
	virtual void DeactivateTurboMode();

	// Override this for the Movement Component logic to work.
	virtual UPawnMovementComponent* GetMovementComponent() const override;

	bool IsCurrentlyFlying() const;

protected:
	virtual void OnTurboModeActivated();
	virtual void OnTurboModeDeactivated();


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

public:
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Returns true if this spacecraft has not been destroyed yet. */
	UFUNCTION(BlueprintPure, Category = "Spacecraft | Survivability")
	bool IsNotDestroyed() const;

protected:
	virtual void DestroySpacecraft();

	void PlayDestroyEffects();

private:
	void CheckShieldStatus();
	void BeginShieldRechargeProcess();
	void StopShieldRechargeProcess();
	bool ShouldIncrementShieldEnergyPoints();


	/**********************************
			 WEAPONS INTERFACE
	**********************************/

public:
	/** Activates primary weapons on the spacecraft. */
	virtual void BeginFiringPrimaryWeapons();

	/** Deactivates primary weapons on the spacecraft. */
	virtual void EndFiringPrimaryWeapons();

private:
	void RotateSpacecraftClockwise(FRotator newRotation);
	void RotateSpacecraftCounterclockwise(FRotator newRotation);
	void StopRotatingSpacecraft();
	void StopMovingSpacecraft();

protected:
	/** Constructs and attaches the weapons to the spacecraft. */
	virtual void InitializeWeaponry();

	/** Destructs the weapons attached to the spacecraft. */
	virtual void DestroyWeaponry();

private:
	/** Will trigger the firing of each primary weapon held by this spacecraft. */
	void FirePrimaryWeapons_Internal();

	/** Checks if any type of weapon should be fired at the moment. */
	void CheckIfWeaponsNeedToBeFired();


	/**********************************
				 GETTERS
	**********************************/

public:
	UStaticMeshComponent* GetSpacecraftMeshComponent() const { return SpacecraftMeshComponent; }
	ESpacecraftFaction    GetFaction()                 const { return Faction; }
};

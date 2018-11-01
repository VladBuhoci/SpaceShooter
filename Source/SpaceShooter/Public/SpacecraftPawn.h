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
	/** The mesh of the spacecraft. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SpacecraftMeshComponent;

	/** Movement component governing the movement of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* SpacecraftMovementComponent;

	/** True if the spacecraft is flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsMovingForward;

	/** True if the spacecraft is flying backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsMovingBackward;

	/**
	 * True if the spacecraft is in turbo mode, significantly increasing the movement speed of the spacecraft.
	 * Only available for forward flight.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsTurboModeActive;

	/** Maximum speed of the spacecraft while turbo mode is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardMaxSpeed;

	/** Maximum speed of the spacecraft while flying normally. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardMaxTurboSpeed;

	/** Current maximum speed of the spacecraft when flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardSpeed;

	/** Current maximum speed of the spacecraft when flying backward. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveBackwardSpeed;

	/** A higher value means the spacecraft turns left or right quicker. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Movement", Meta = (AllowPrivateAccess = "true", ClampMin = "8", UIMin = "8", ClampMax = "20", UIMax = "20"))
	float SpacecraftTurnSpeed;

	/** Particle system which is activated when the spacecraft is moving forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* BackSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is moving backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FrontSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is rotating clockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* LeftSideThrusterParticleEmitter;

	/** Particle system which is activated when the spacecraft is rotating counterclockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* RightSideThrusterParticleEmitter;

	/** Particle system template for normal forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BacksideNormalThrusterParticleSystem;

	/** Particle system template for turbo forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Effects", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BacksideTurboThrusterParticleSystem;

	/** Maximum amount of damage this spacecraft can take before it gets destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability", Meta = (AllowPrivateAccess = "true"))
	int32 MaxHitPoints;

	/** Current amount of damage this spacecraft can take before it gets destroyed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Survivability", Meta = (AllowPrivateAccess = "true"))
	int32 CurrentHitPoints;

	/** Primary weapon class. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> PrimaryWeaponClass;
	
	/** Primary weapon reference. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (AllowPrivateAccess = "true"))
	AWeapon* PrimaryWeapon;

	/** If true, all attached primary weapons will be fired together. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Weapons", Meta = (AllowPrivateAccess = "true"))
	bool bIsFiringPrimaryWeapons;

	/** The "side" this spacecraft belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Misc", Meta = (AllowPrivateAccess = "true"))
	ESpacecraftFaction Faction;

public:
	/** Sets default values. */
	ASpacecraftPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


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

protected:
	virtual void OnTurboModeActivated();
	virtual void OnTurboModeDeactivated();


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

public:
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void DestroySpacecraft();

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

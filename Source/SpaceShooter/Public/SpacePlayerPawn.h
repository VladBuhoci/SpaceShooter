// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpacePlayerPawn.generated.h"

// Forward declarations.
class USpringArmComponent;
class UCameraComponent;
class UFloatingPawnMovement;
class UParticleSystemComponent;

class AProjectile;
class AWeapon;

/**
 * Represents the player in the space levels.
 * It is instantiated whenever a playable level is loaded.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerPawn : public APawn
{
	GENERATED_BODY()

private:
	/**
	 * A component that is not rendered in game.
	 * This object attaches to the spaceship while maintaining a zero-rotation relative to the world.
	 * The camera's spring arm focuses on this component, therefore the camera will not rotate when the spaceship does so.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* CentralPlayerSceneComponent;

	/** The mesh of the spaceship. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SpaceshipMeshComponent;

	/** Movement component governing the movement of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UFloatingPawnMovement* SpaceshipMovementComponent;

	/** True if the spaceship is flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsMovingForward;

	/** True if the spaceship is flying backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsMovingBackward;

	/**
	 * True if the spaceship is in turbo mode, significantly increasing the movement speed of the spaceship.
	 * Only available for forward flight.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	bool bIsTurboModeActive;

	/** Maximum speed of the spaceship while turbo mode is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardMaxSpeed;

	/** Maximum speed of the spaceship while flying normally. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardMaxTurboSpeed;

	/** Current maximum speed of the spaceship when flying forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveForwardSpeed;

	/** Current maximum speed of the spaceship when flying backward. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true"))
	float MoveBackwardSpeed;

	/** The camera that is used to view the player and the world it belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	/**
	 * The spring arm is used to keep the camera away at a certain distance from the player,
	 *		retracting or extending when (opaque) obstacles of any sort obstruct the camera's view.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	/** Spring Arm Component offset relative to the Central Player Scene Component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	FVector SpringArmOffset;

	/** Spring Arm Component rotation relative to the Central Player Scene Component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	FRotator SpringArmRotation;

	/** Spring arm's length (distance from the target - spaceship - to the camera) while turbo mode is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float SpringArmTurboLength;

	/** A higher value means the spaceship turns left or right quicker. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Movement", Meta = (AllowPrivateAccess = "true", ClampMin = "8", UIMin = "8", ClampMax = "20", UIMax = "20"))
	float SpaceshipTurnSpeed;

	/** Particle system which is activated when the spaceship is moving forward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* BackSideThrusterParticleEmitter;

	/** Particle system which is activated when the spaceship is moving backward. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* FrontSideThrusterParticleEmitter;

	/** Particle system which is activated when the spaceship is rotating clockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* LeftSideThrusterParticleEmitter;

	/** Particle system which is activated when the spaceship is rotating counterclockwise. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Components", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* RightSideThrusterParticleEmitter;

	/** Particle system template for normal forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Effects", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BacksideNormalThrusterParticleSystem;

	/** Particle system template for turbo forward flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Effects", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BacksideTurboThrusterParticleSystem;

	/** Primary weapon class. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Player | Weapons", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> PrimaryWeaponClass;
	
	/** Primary weapon reference. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Weapons", Meta = (AllowPrivateAccess = "true"))
	AWeapon* PrimaryWeapon;

	/** If true, all attached primary weapons will be fired together. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Weapons", Meta = (AllowPrivateAccess = "true"))
	bool bIsFiringPrimaryWeapons;

public:
	/** Sets default values. */
	ASpacePlayerPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:
	/**********************************
		     MOVEMENT INTERFACE
	**********************************/

	void MoveForward(float Value);
	void MoveBackward(float Value);
	void RotateSpaceship(FRotator rotator);
	void ActivateTurboMode();
	void DeactivateTurboMode();

	// Override this for the Movement Component logic to work.
	virtual UPawnMovementComponent* GetMovementComponent() const override;


	/**********************************
		     WEAPONS INTERFACE
	**********************************/

	/** Activates primary weapons on the spaceship. */
	void BeginFiringPrimaryWeapons();

	/** Deactivates primary weapons on the spaceship. */
	void EndFiringPrimaryWeapons();

private:	
	void RotateSpaceshipClockwise(FRotator newRotation);
	void RotateSpaceshipCounterclockwise(FRotator newRotation);
	void StopRotatingSpaceship();
	void StopMovingSpaceship();

	/** Constructs and attaches the weapons to the spaceship. */
	void InitializeWeaponry();

	/** Will trigger the firing of each primary weapon held by this spaceship. */
	void FirePrimaryWeapons_Internal();

	/** Checks if any type of weapon should be fired at the moment. */
	void CheckIfWeaponsNeedToBeFired();

public:
	/**********************************
				GETTERS
	**********************************/

	UStaticMeshComponent* GetSpaceshipMeshComponent() const	{ return SpaceshipMeshComponent; }
};

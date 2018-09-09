// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "SpacecraftPawn.h"

#include "CoreMinimal.h"
#include "SpacePlayerPawn.generated.h"

// Forward declarations.
class USpringArmComponent;
class UCameraComponent;
class UParticleSystemComponent;


/**
 * Represents the player in the space levels.
 * It is instantiated whenever a playable level is loaded.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerPawn : public ASpacecraftPawn
{
	GENERATED_BODY()

private:
	/** The camera that is used to view the player and the world it belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	/**
	 * The spring arm is used to keep the camera away at a certain distance from the player,
	 *		retracting or extending when (opaque) obstacles of any sort obstruct the camera's view.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	/** Spring Arm Component offset relative to the Central Scene Component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	FVector SpringArmOffset;

	/** Spring Arm Component rotation relative to the Central Scene Component. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	FRotator SpringArmRotation;

	/** Spring arm's length (distance from the target - spacecraft - to the camera) while turbo mode is active. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float SpringArmTurboLength;

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

	/*virtual void MoveForward(float Value) override;
	virtual void MoveBackward(float Value) override;
	virtual void RotateSpacecraft(FRotator rotator) override;
	virtual void ActivateTurboMode();
	virtual void DeactivateTurboMode() override;*/

protected:
	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;


	/**********************************
		     WEAPONS INTERFACE
	**********************************/

public:
	/** Activates primary weapons on the spacecraft. */
	virtual void BeginFiringPrimaryWeapons() override;

	/** Deactivates primary weapons on the spacecraft. */
	virtual void EndFiringPrimaryWeapons() override;

public:
	/**********************************
				GETTERS
	**********************************/


};

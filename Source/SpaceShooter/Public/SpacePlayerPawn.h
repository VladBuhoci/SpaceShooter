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
	/**
	 * A component that is not rendered in game.
	 * This object attaches to the spaceship while maintaining a zero-rotation relative to the world.
	 * For the SpacePlayerPawn subclass, this is very useful because the camera's spring arm focuses on this component,
	 *		therefore the camera will not rotate when the spaceship does so.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* CentralSceneComponent;

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

public:
	/** Sets default values. */
	ASpacePlayerPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


	/**********************************
		     MOVEMENT INTERFACE
	**********************************/

protected:
	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

protected:
	/**
	 * Called right before the spacecraft is removed from the world.
	 * 
	 * @param bShouldPlayDestroyEffects [ref] if set to true, an explosion will be spawned at the spacecraft's location.
	 * @param bShouldBeDestroyedForGood [ref] if set to false, the attempt to remove this actor is canceled.
	 */
	virtual void PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed) override;


	/**********************************
		     WEAPONS INTERFACE
	**********************************/

public:
	/** Activates primary weapons on the spacecraft. */
	virtual void BeginFiringWeapon() override;

	/** Deactivates primary weapons on the spacecraft. */
	virtual void EndFiringWeapon() override;

	/**********************************
				GETTERS
	**********************************/

public:


	/**********************************
			 CAMERA INTERFACE
	**********************************/

private:
	/** The targeted length of the camera's spring arm. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float DesiredCameraSpringArmLength;

	/** The speed at which the spring arm of the camera changes its length during flight. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed;

	void CheckCameraOffset(float DeltaTime);
};

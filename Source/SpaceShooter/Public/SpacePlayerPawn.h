// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpacePlayerPawn.generated.h"

// Forward declarations.
class USpringArmComponent;
class UCameraComponent;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player", Meta = (AllowPrivateAccess = "true"))
	USceneComponent * CentralPlayerSceneComponent;

	/** The mesh of the spaceship. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent * SpaceshipMeshComponent;

	/**
	 * The spring arm is used to keep the camera away at a certain distance from the player,
	 *		retracting or extending when (opaque) obstacles of any sort obstruct the camera's view.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	USpringArmComponent * SpringArmComponent;

	/** The camera that is used to view the player and the world it belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player | Player Camera", Meta = (AllowPrivateAccess = "true"))
	UCameraComponent * CameraComponent;

	/** Movement component governing the movement of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player", Meta = (AllowPrivateAccess = "true"))
	class UFloatingPawnMovement * SpaceshipMovementComponent;

public:
	/** Sets default values. */
	ASpacePlayerPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	// Movement methods.
public:
	void RotateShip(FRotator rotator);
	void MoveForward(float Value);
	void MoveBackward(float Value);

	virtual UPawnMovementComponent * GetMovementComponent() const override;

private:	
	void RotateShipClockwise();
	void RotateShipCounterclockwise();
	// ~ end of movement methods.

	// GETTERS
public:
	UStaticMeshComponent * GetSpaceshipMeshComponent() const	{ return SpaceshipMeshComponent; }
};

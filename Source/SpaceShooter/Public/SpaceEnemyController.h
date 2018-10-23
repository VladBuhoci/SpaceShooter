// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SpaceEnemyController.generated.h"

// Forward declarations.
class ASpaceEnemyPawn;
class UNavigationSystem;

UENUM(BlueprintType)
enum class ESpacecraftState : uint8
{
	Idle,
	Flying
};


/**
 * Space Enemy Pawn AI Controller.
 */
UCLASS()
class SPACESHOOTER_API ASpaceEnemyController : public AAIController
{
	GENERATED_BODY()
	
private:
	/** The navigation system is used to gather navigation mesh data. */
	UNavigationSystem* NavSystem;

	/** The controlled enemy pawn of this controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	ASpaceEnemyPawn* PossessedSpacePawn;

	/** Minimum time to stay in place before moving elsewhere (used only when wandering). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	float IdleTimeMin;

	/** Maximum time to stay in place before moving elsewhere (used only when wandering). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	float IdleTimeMax;

	/**
	 * Distance from origin (pawn location) to edge of navigable space (used only when wandering).
	 * Higher values result in a larger explorable area for this pawn.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	float WanderAreaRadius;

	/** The targeted location of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	FVector TargetLocation;

	/** The targeted location of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	ESpacecraftState SpacecraftState;

public:
	/** Sets default values. */
	ASpaceEnemyController();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/** Called on completing current movement request. */
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result);

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


	/**********************************
			 WEAPON INTERFACE
	**********************************/

protected:
	/** Sends a request to the spaceship to activate its primary weapons. */
	void BeginFiringPrimaryWeapons();

	/** Sends a request to the spaceship to deactivate its primary weapons. */
	void EndFiringPrimaryWeapons();


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

private:
	/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
	void HandleSpaceshipRotation();
	void BeginFlightToRandomLocation();

	void MovePawnForward(float Value);
	void MovePawnBackward(float Value);

	void ActivateTurboMode();
	void DeactivateTurboMode();

	FVector GetNewRandomLocationInNavMesh();
};

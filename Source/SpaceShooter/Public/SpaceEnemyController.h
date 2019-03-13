// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SpaceEnemyController.generated.h"

// Forward declarations.
class ASpaceEnemyPawn;
class ASpacecraftPawn;
class UNavigationSystem;


UENUM(BlueprintType)
enum class ESpacecraftState : uint8
{
	Idle,
	ScheduledForFlying,
	Flying,
	Attacking
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

	/** Timer Handle used to work with timers. */
	FTimerHandle TimerHandle;

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
	FVector LocationToReach;

	/** The targeted spacecraft of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Enemy Controller", Meta = (AllowPrivateAccess = "true"))
	ASpacecraftPawn* SpacecraftToReach;

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
	void BeginFiringWeapon();

	/** Sends a request to the spaceship to deactivate its primary weapons. */
	void EndFiringWeapon();


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

public:
	void AttemptAttackOnPlayer(ASpacecraftPawn* SpacecraftToFollow);
	void StayInPlace(bool bContinueAttack);

private:
	void HandleSpaceshipRotation(FVector TargetLocation);
	void ScheduleNewRandomFlight();
	void BeginFlightToRandomLocation();
	void EnterCompletelyIdleState();

	void MovePawnForward(float Value);
	void MovePawnBackward(float Value);

	void ActivateTurboMode();
	void DeactivateTurboMode();

	FVector GetNewRandomLocationInNavMesh();
};

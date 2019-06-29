// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Controllers/SpaceEnemyController.h"
#include "Pawns/SpaceEnemyPawn.h"
#include "GameModes/SpaceGameMode.h"

#include "Kismet/KismetMathLibrary.h"

#include "AI/Navigation/NavigationSystem.h"

#include "Engine/World.h"

#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


ASpaceEnemyController::ASpaceEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;

	IdleTimeMin                  = 0.1f;
	IdleTimeMax                  = 3.0f;
	WanderAreaRadius             = 3000.0f;
	SpacecraftState              = ESpacecraftState::Idle;
	CloseEnemySearchTimeInterval = 1.0f;
}

void ASpaceEnemyController::BeginPlay()
{
	Super::BeginPlay();

	// Cast the pawn to our custom enemy type so we can make use of its public methods.
	PossessedSpacePawn = Cast<ASpaceEnemyPawn>(GetPawn());

	UWorld* World = GetWorld();
	if (World)
	{
		// Store the game mode for later usage.
		SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(World));

		NavSystem = World->GetNavigationSystem();
	}
}

void ASpaceEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PossessedSpacePawn)
	{
		switch (SpacecraftState)
		{
		case ESpacecraftState::Idle:
			EnterCompletelyIdleState();

			ScheduleNewRandomFlight();
			ScheduleSearchingForNewTarget();

			break;

		case ESpacecraftState::Flying:
			HandleSpaceshipRotation(LocationToReach);

			// TODO: maybe we can have the spacecrafts behave smarter by letting them can go backward too.
			PossessedSpacePawn->MaintainForwardMovementSetup();

			break;

		case ESpacecraftState::Attacking:
			// Only handle rotation and movement if the target ship is not completely destroyed yet.
			if (SpacecraftToReach && SpacecraftToReach->IsNotDestroyed())
			{
				HandleSpaceshipRotation(SpacecraftToReach->GetActorLocation());

				PossessedSpacePawn->MaintainForwardMovementSetup();
			}
			// Else abandon current target-based movement (it will make this AI fly around randomly until another target shows up).
			else
			{
				if (PossessedSpacePawn->IsCurrentlyFlying())
				{
					StayInPlace(false);
				}
				else
				{
					EnterCompletelyIdleState();
				}
			}

			break;
		}
	}
	else
	{
		PossessedSpacePawn = Cast<ASpaceEnemyPawn>(GetPawn());
	}
}

void ASpaceEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (SpacecraftState != ESpacecraftState::Attacking)
	{
		EnterCompletelyIdleState();
	}
}

void ASpaceEnemyController::BeginFiringWeapon()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->BeginFiringWeapon();
	}
}

void ASpaceEnemyController::EndFiringWeapon()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->EndFiringWeapon();
	}
}

void ASpaceEnemyController::NotifyController(ESpaceControllerNotification Notification, ASpacecraftPawn* OtherPawn)
{
	switch (Notification)
	{
	case ESpaceControllerNotification::TargetFound:
		AttemptAttackOnSpacecraft(OtherPawn);
		break;

	case ESpaceControllerNotification::TargetOutOfCombatArea:
		GoAfterTarget();
		break;

	case ESpaceControllerNotification::TargetInsideCombatArea:
		StayInPlace(true);
		break;

	case ESpaceControllerNotification::TargetLost:
		StayInPlace(false);
		LookForAndAttackEnemyInCloseProximity();
		break;

	default:
		StayInPlace(false);
		break;
	}
}

void ASpaceEnemyController::AttemptAttackOnSpacecraft(ASpacecraftPawn* SpacecraftToFollow)
{
	if (SpacecraftToFollow)
	{
		// Clear the timers in case there was a MoveTo or SearchNewTarget action scheduled in the near future.
		UnscheduleNewRandomFlight();
		UnscheduleSearchingForNewTarget();

		SpacecraftToReach = SpacecraftToFollow;
		SpacecraftState   = ESpacecraftState::Attacking;

		FTimerHandle ShootDelayTimerHandle;
		float Delay = 0.5f;

		// Don't start shooting immediately, wait until the ship is completely (sort of) rotated towards the target,
		//		because it looks weird otherwise (shooting in the wrong direction).
		GetWorldTimerManager().SetTimer(ShootDelayTimerHandle, this, &ASpaceEnemyController::BeginFiringWeapon, Delay, false);

		GoAfterTarget();
	}
	else
	{
		EnterCompletelyIdleState();
	}
}

void ASpaceEnemyController::GoAfterTarget()
{
	if (SpacecraftToReach)
	{
		MoveToActor(SpacecraftToReach, 10.0f);
	}
}

void ASpaceEnemyController::StayInPlace(bool bContinueAttack)
{
	UnscheduleNewRandomFlight();

	SpacecraftState = bContinueAttack ? ESpacecraftState::Attacking : ESpacecraftState::Idle;

	StopMovement();
}

void ASpaceEnemyController::HandleSpaceshipRotation(FVector TargetLocation)
{
	FRotator TargetRotation;

	TargetRotation       = (TargetLocation - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
	TargetRotation.Pitch = 0.0f;

	PossessedSpacePawn->RotateSpacecraft(TargetRotation);
}

void ASpaceEnemyController::ScheduleNewRandomFlight()
{
	SpacecraftState = ESpacecraftState::ScheduledForFlying;

	float IdleTime = UKismetMathLibrary::RandomFloatInRange(IdleTimeMin, IdleTimeMax);

	GetWorldTimerManager().SetTimer(NonAggressiveStateMovementTimerHandle,
		this, &ASpaceEnemyController::BeginFlightToRandomLocation, IdleTime, false);
}

void ASpaceEnemyController::UnscheduleNewRandomFlight()
{
	GetWorldTimerManager().ClearTimer(NonAggressiveStateMovementTimerHandle);
}

void ASpaceEnemyController::BeginFlightToRandomLocation()
{
	LocationToReach = GetNewRandomLocationInNavMesh();
	SpacecraftState = ESpacecraftState::Flying;

	MoveToLocation(LocationToReach);
}

void ASpaceEnemyController::EnterCompletelyIdleState()
{
	UnscheduleNewRandomFlight();
	UnscheduleSearchingForNewTarget();

	SpacecraftToReach = nullptr;
	SpacecraftState   = ESpacecraftState::Idle;

	EndFiringWeapon();
}

void ASpaceEnemyController::ScheduleSearchingForNewTarget()
{
	// When we're idle/flying randomly (so, without a target to attack), we make a query to see if there are any targets
	//		already sitting in our detection area (normally found only when they enter the area).

	GetWorldTimerManager().SetTimer(IdleSearchForEnemiesInCloseProximityTimerHandle,
		this, &ASpaceEnemyController::LookForAndAttackEnemyInCloseProximity, CloseEnemySearchTimeInterval, false);
}

void ASpaceEnemyController::UnscheduleSearchingForNewTarget()
{
	GetWorldTimerManager().ClearTimer(IdleSearchForEnemiesInCloseProximityTimerHandle);
}

void ASpaceEnemyController::MovePawnForward(float Value)
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->MoveForward(Value);
	}
}

void ASpaceEnemyController::MovePawnBackward(float Value)
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->MoveBackward(Value);
	}
}

void ASpaceEnemyController::ActivateTurboMode()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->ActivateTurboMode();
	}
}

void ASpaceEnemyController::DeactivateTurboMode()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->DeactivateTurboMode();
	}
}

FVector ASpaceEnemyController::GetNewRandomLocationInNavMesh()
{
	if (PossessedSpacePawn && NavSystem)
	{
		FNavLocation RandomNavLocation;

		if (NavSystem->GetRandomReachablePointInRadius(PossessedSpacePawn->GetActorLocation(), WanderAreaRadius, RandomNavLocation))
		{
			return RandomNavLocation.Location;
		}
	}

	return PossessedSpacePawn != NULL ? PossessedSpacePawn->GetActorLocation() : FVector::ZeroVector;
}

void ASpaceEnemyController::LookForAndAttackEnemyInCloseProximity()
{
	if (!PossessedSpacePawn)
		return;

	// We don't search for a new target if we're already attacking one.
	if (SpacecraftState == ESpacecraftState::Attacking)
		return;

	ASpacecraftPawn* FoundTarget = FindEnemyAroundThisPawn(PossessedSpacePawn->IsAlwaysAggressive());

	if (FoundTarget)
	{
		AttemptAttackOnSpacecraft(FoundTarget);
	}
}

ASpacecraftPawn* ASpaceEnemyController::FindEnemyAroundThisPawn(bool bSearchEntireWorldForEnemies)
{
	if (!PossessedSpacePawn)
		return nullptr;

	ESpacecraftFaction OppositeFaction = PossessedSpacePawn->GetFaction() == ESpacecraftFaction::Human
		? ESpacecraftFaction::Clone
		: ESpacecraftFaction::Human;

	TArray<ASpacecraftPawn*> FoundShips = bSearchEntireWorldForEnemies
		? TArray<ASpacecraftPawn*>(SpaceGameMode->GetAllSpacecraftsInWorld())
		: PossessedSpacePawn->GetAllSpacecraftsInDetectionArea(OppositeFaction);
	
	FoundShips.Remove(PossessedSpacePawn);	// remove our pawn from the array.

	// Iterate through the array and remove all spacecrafts belonging to the same faction as this controller's pawn.
	for (auto ShipsIterator = FoundShips.CreateIterator(); ShipsIterator; ++ShipsIterator)
	{
		if ((* ShipsIterator)->GetFaction() != OppositeFaction)
		{
			ShipsIterator.RemoveCurrent();
		}
	}

	if (FoundShips.Num() == 0)
		return nullptr;

	// TODO: make this a clever and slightly random pick up choice (i.e.: pick randomly from 3 closest enemies).
	int32 ChosenShipIndex = 0;

	return FoundShips[ChosenShipIndex];
}

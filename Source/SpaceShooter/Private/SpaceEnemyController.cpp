// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceEnemyController.h"
#include "SpaceEnemyPawn.h"

#include "Kismet/KismetMathLibrary.h"

#include "AI/Navigation/NavigationSystem.h"

#include "Engine/World.h"

#include "Runtime/Engine/Public/TimerManager.h"


ASpaceEnemyController::ASpaceEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;

	IdleTimeMin      = 3.0f;
	IdleTimeMax      = 10.0f;
	WanderAreaRadius = 3000.0f;
	SpacecraftState  = ESpacecraftState::Idle;
}

void ASpaceEnemyController::BeginPlay()
{
	Super::BeginPlay();

	// Cast the pawn to our custom enemy type so we can make use of its public methods.
	PossessedSpacePawn = Cast<ASpaceEnemyPawn>(GetPawn());

	UWorld* World = GetWorld();
	if (World)
	{
		NavSystem = World->GetNavigationSystem();
	}

	BeginFlightToRandomLocation();
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
					ScheduleNewRandomFlight();
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
		ScheduleNewRandomFlight();
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

void ASpaceEnemyController::AttemptAttackOnPlayer(ASpacecraftPawn* SpacecraftToFollow)
{
	// Clear the timer in case there was a MoveTo action scheduled in the near future.
	GetWorldTimerManager().ClearTimer(TimerHandle);

	MoveToActor(SpacecraftToFollow, 10.0f);

	SpacecraftToReach = SpacecraftToFollow;
	SpacecraftState   = ESpacecraftState::Attacking;

	FTimerHandle ShootDelayTimerHandle;
	float Delay = 0.5f;

	// Don't start shooting immediately because it looks weird until the ship is completely rotated towards the target.
	GetWorldTimerManager().SetTimer(ShootDelayTimerHandle, this, &ASpaceEnemyController::BeginFiringWeapon, Delay, false);
}

void ASpaceEnemyController::StayInPlace(bool bContinueAttack)
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

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

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceEnemyController::BeginFlightToRandomLocation, IdleTime, false);
}

void ASpaceEnemyController::BeginFlightToRandomLocation()
{
	LocationToReach = GetNewRandomLocationInNavMesh();
	SpacecraftState = ESpacecraftState::Flying;

	MoveToLocation(LocationToReach);
}

void ASpaceEnemyController::EnterCompletelyIdleState()
{
	GetWorldTimerManager().ClearTimer(TimerHandle);

	SpacecraftToReach = nullptr;
	SpacecraftState   = ESpacecraftState::Idle;

	EndFiringWeapon();
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

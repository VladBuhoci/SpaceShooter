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
		if (SpacecraftState == ESpacecraftState::Flying)
		{
			HandleSpaceshipRotation(LocationToReach);
			
			// TODO: maybe we can have the spacecrafts behave smarter by letting them can go backward too.
			PossessedSpacePawn->MaintainForwardMovementSetup();
		}
		else if (SpacecraftState == ESpacecraftState::Attacking)
		{
			if (SpacecraftToReach)
			{
				HandleSpaceshipRotation(SpacecraftToReach->GetActorLocation());
				PossessedSpacePawn->MaintainForwardMovementSetup();
			}
			else
			{
				StopMovement();
			}
		}
	}
}

void ASpaceEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if (SpacecraftState != ESpacecraftState::Attacking)
	{
		SpacecraftToReach = nullptr;
		SpacecraftState   = ESpacecraftState::Idle;

		float IdleTime = UKismetMathLibrary::RandomFloatInRange(IdleTimeMin, IdleTimeMax);

		GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceEnemyController::BeginFlightToRandomLocation, IdleTime, false);
	}
}

void ASpaceEnemyController::BeginFiringPrimaryWeapons()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->BeginFiringPrimaryWeapons();
	}
}

void ASpaceEnemyController::EndFiringPrimaryWeapons()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->EndFiringPrimaryWeapons();
	}
}

void ASpaceEnemyController::AttemptAttackOnPlayer(ASpacecraftPawn* SpacecraftToFollow)
{
	// Clear the timer in case there was a MoveTo action scheduled in the near future.
	GetWorldTimerManager().ClearTimer(TimerHandle);

	MoveToActor(SpacecraftToFollow, 10.0f);

	SpacecraftToReach = SpacecraftToFollow;
	SpacecraftState   = ESpacecraftState::Attacking;
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

	TargetRotation = (TargetLocation - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
	TargetRotation.Pitch = 0.0f;

	PossessedSpacePawn->RotateSpacecraft(TargetRotation);
}

void ASpaceEnemyController::BeginFlightToRandomLocation()
{
	LocationToReach = GetNewRandomLocationInNavMesh();

	MoveToLocation(LocationToReach);
	
	SpacecraftState = ESpacecraftState::Flying;
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

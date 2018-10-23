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

	if (SpacecraftState == ESpacecraftState::Flying)
	{
		if (PossessedSpacePawn)
		{
			HandleSpaceshipRotation();

			// TODO: maybe we can have the spacecrafts behave smarter by letting them can go backward too.
			PossessedSpacePawn->MaintainForwardMovementSetup();
		}
	}
}

void ASpaceEnemyController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult & Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	SpacecraftState = ESpacecraftState::Idle;

	FTimerHandle TimerHandle;
	float IdleTime = UKismetMathLibrary::RandomFloatInRange(IdleTimeMin, IdleTimeMax);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASpaceEnemyController::BeginFlightToRandomLocation, IdleTime, false);
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

void ASpaceEnemyController::HandleSpaceshipRotation()
{
	FRotator TargetRotation;

	TargetRotation = (TargetLocation - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
	TargetRotation.Pitch = 0.0f;

	PossessedSpacePawn->RotateSpacecraft(TargetRotation);
}

void ASpaceEnemyController::BeginFlightToRandomLocation()
{
	TargetLocation = GetNewRandomLocationInNavMesh();

	MoveToLocation(TargetLocation);
	
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

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceEnemyPawn.h"
#include "SpaceEnemyController.h"
#include "SpacePlayerPawn.h"
#include "LootChest.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"


ASpaceEnemyPawn::ASpaceEnemyPawn()
{
	AIControllerClass               = ASpaceEnemyController::StaticClass();
	AutoPossessPlayer               = EAutoReceiveInput::Disabled;
	AutoPossessAI                   = EAutoPossessAI::PlacedInWorldOrSpawned;

	DetectionArea                   = CreateDefaultSubobject<USphereComponent>("Detection Area");
	CloseQuartersArea               = CreateDefaultSubobject<USphereComponent>("Close Quarters Area");
	
	MoveForwardMaxTurboSpeed        = 1400.0f;
	MoveForwardMaxSpeed             = 900.0f;
	MoveBackwardSpeed               = 600.0f;
	MaxHitPoints                    = 50.0f;
	MaxShieldPoints                 = 100.0f;
	ShieldRechargeRate              = 5.0f;
	ShieldRechargeDelay             = 2.5f;
	Faction                         = ESpacecraftFaction::Clone;
	DetectionAreaRadius             = 1500.0f;
	CloseQuartersAreaRadius         = 500.0f;
	DetectionAreaRadiusModifier     = 135.0f;
	CloseQuartersAreaRadiusModifier = 150.0f;
	LootBoundingBox.Min             = FVector(-75.0f, -50.0f, 20.0f);
	LootBoundingBox.Max             = FVector( 75.0f,  50.0f, 40.0f);
	
	DetectionArea->SetupAttachment(SpacecraftMeshComponent);
	DetectionArea->SetSphereRadius(DetectionAreaRadius);
	DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectEnterDetectionArea);
	DetectionArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectExitDetectionArea);
	
	CloseQuartersArea->SetupAttachment(SpacecraftMeshComponent);
	CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius);
	CloseQuartersArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectEnterCloseQuartersArea);
	CloseQuartersArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectExitCloseQuartersArea);
}

void ASpaceEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

}

void ASpaceEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpaceEnemyPawn::ExecuteOnObjectEnterDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacePlayerPawn* Player = Cast<ASpacePlayerPawn>(OtherActor);

		// Acquire a target only if there isn't one at the moment.
		if (Player && Target == NULL)
		{
			Target = Player;

			ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
			if (SpaceEnemyController)
			{
				DetectionArea->SetSphereRadius(DetectionAreaRadius * DetectionAreaRadiusModifier / 100.0f);

				SpaceEnemyController->AttemptAttackOnPlayer(Target);
			}
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectExitDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacePlayerPawn* Player = Cast<ASpacePlayerPawn>(OtherActor);

		// Stop hunting only if the target spacecraft has gone out of the detection area.
		if (Player && Target == Player)
		{
			Target = nullptr;

			ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
			if (SpaceEnemyController)
			{
				DetectionArea->SetSphereRadius(DetectionAreaRadius);

				SpaceEnemyController->StayInPlace(false);
			}
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectEnterCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacePlayerPawn* Player = Cast<ASpacePlayerPawn>(OtherActor);

		// Make sure this spacecraft keeps attacking the same target.
		if (Player && Target == Player)
		{
			ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
			if (SpaceEnemyController)
			{
				CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius * CloseQuartersAreaRadiusModifier / 100.0f);

				SpaceEnemyController->StayInPlace(true);
			}
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectExitCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacePlayerPawn* Player = Cast<ASpacePlayerPawn>(OtherActor);

		// Make sure this spacecraft keeps attacking the same target.
		if (Player && Target == Player)
		{
			ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
			if (SpaceEnemyController)
			{
				CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius);

				SpaceEnemyController->AttemptAttackOnPlayer(Target);
			}
		}
	}
}

void ASpaceEnemyPawn::OnTurboModeActivated()
{

}

void ASpaceEnemyPawn::OnTurboModeDeactivated()
{

}

// TODO: W.I.P.
void ASpaceEnemyPawn::PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed)
{
	// ... anything to add here? (besides triggering a loot chest spawn event)

	// Attempt to create loot chests.
	TryToCreateLootChests();

	bShouldPlayDestroyEffects = true;
	bShouldBeDestroyed        = true;
}

void ASpaceEnemyPawn::BeginFiringWeapon()
{
	Super::BeginFiringWeapon();

}

void ASpaceEnemyPawn::EndFiringWeapon()
{
	Super::EndFiringWeapon();

}

void ASpaceEnemyPawn::TryToCreateLootChests()
{
	for (FLootChestWithChance_KeyValue_Pair ChestAndItsChance : LootChestsAndChances)
	{
		if (ChestAndItsChance.ChestType != NULL && ChestAndItsChance.SpawnChance > 0)
		{
			// Get a random number between 1 and 100.
			int RandomNumber = FMath::RandRange(1, 100);

			if (RandomNumber <= ChestAndItsChance.SpawnChance)
			{
				SpawnLootChest(ChestAndItsChance.ChestType);
			}
		}
	}
}

void ASpaceEnemyPawn::SpawnLootChest(TSubclassOf<ALootChest> ChestTypeToSpawn)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FTransform ChestTransform;
		FVector    ChestLocation = GetActorLocation() + FMath::RandPointInBox(LootBoundingBox);
		FRotator   ChestRotation = GetActorRotation();

		// Spawn chest with a random rotation around the Z (up) axis.
		ChestRotation.Yaw += FMath::RandRange(0.0f, 359.9f);

		ChestTransform.SetLocation(ChestLocation);
		ChestTransform.SetRotation(ChestRotation.Quaternion());

		World->SpawnActor<ALootChest>(ChestTypeToSpawn, ChestTransform);
	}
}

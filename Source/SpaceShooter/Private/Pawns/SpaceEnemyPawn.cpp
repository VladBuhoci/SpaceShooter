// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Pawns/SpaceEnemyPawn.h"
#include "Controllers/SpaceEnemyController.h"
#include "Loot/Containers/LootChest.h"
#include "UI/SpaceHUD.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Engine/World.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Kismet/GameplayStatics.h"


ASpaceEnemyPawn::ASpaceEnemyPawn()
{
	AIControllerClass                   = ASpaceEnemyController::StaticClass();
	AutoPossessPlayer                   = EAutoReceiveInput::Disabled;
	AutoPossessAI                       = EAutoPossessAI::PlacedInWorldOrSpawned;

	DetectionArea                       = CreateDefaultSubobject<USphereComponent>("Detection Area");
	CloseQuartersArea                   = CreateDefaultSubobject<USphereComponent>("Close Quarters Area");
	SurvivabilitySummaryWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Survivability Summary Widget Component");
	
	MoveForwardMaxTurboSpeed            = 1400.0f;
	MoveForwardMaxSpeed                 = 900.0f;
	MoveBackwardSpeed                   = 600.0f;
	MaxHitPoints                        = 50.0f;
	MaxShieldPoints                     = 100.0f;
	ShieldRechargeRate                  = 5.0f;
	ShieldRechargeDelay                 = 2.5f;
	Name                                = FText::FromString("Unnamed NPC");
	Faction                             = ESpacecraftFaction::Unspecified;
	bAlwaysAggressive                   = false;
	DetectionAreaRadius                 = 1700.0f;
	CloseQuartersAreaRadius             = 700.0f;
	DetectionAreaRadiusModifier         = 50.0f;
	CloseQuartersAreaRadiusModifier     = 40.0f;
	LootBoundingBox.Min                 = FVector(-75.0f, -50.0f, 20.0f);
	LootBoundingBox.Max                 = FVector( 75.0f,  50.0f, 40.0f);
	SurvivabilityWidgetVisibilityTime   = 1.5f;
	bWaitingForWeaponToCoolDown         = false;
	
	DetectionArea->SetupAttachment(SpacecraftMeshComponent);
	DetectionArea->SetSphereRadius(DetectionAreaRadius);
	DetectionArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectEnterDetectionArea);
	DetectionArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectExitDetectionArea);
	
	CloseQuartersArea->SetupAttachment(SpacecraftMeshComponent);
	CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius);
	CloseQuartersArea->OnComponentBeginOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectEnterCloseQuartersArea);
	CloseQuartersArea->OnComponentEndOverlap.AddDynamic(this, &ASpaceEnemyPawn::ExecuteOnObjectExitCloseQuartersArea);

	SurvivabilitySummaryWidgetComponent->SetupAttachment(SpacecraftMeshComponent);
	SurvivabilitySummaryWidgetComponent->PrimaryComponentTick.bCanEverTick = true;	// false by default, no clue why, but it won't render the widget that way.
	SurvivabilitySummaryWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);		// Draw using screen coordinates so the widget looks 2D, not 3D.
	SurvivabilitySummaryWidgetComponent->SetDrawSize(FVector2D(150.0f, 50.0f));		// Size of the rectangular surface the widget is drawn on.
	SurvivabilitySummaryWidgetComponent->SetPivot(FVector2D(0.5f, 2.5f));			// Offset from origin of this actor's local space.
	SurvivabilitySummaryWidgetComponent->SetVisibility(false);

	// Ammunition stocks:
	AmmoPools.Add(EWeaponType::Blaster , FAmmunitionStock(true));
	AmmoPools.Add(EWeaponType::Cannon  , FAmmunitionStock(true));
	AmmoPools.Add(EWeaponType::Volley  , FAmmunitionStock(true));
	AmmoPools.Add(EWeaponType::Launcher, FAmmunitionStock(true));
}

void ASpaceEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius);

	UpdateStateBasedOnAggressiveness();
}

void ASpaceEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpaceEnemyPawn::ExecuteOnObjectEnterDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Don't lock on a new target if we already have one to attack.
	if (Target != NULL)
		return;

	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacecraftPawn* NewTarget = Cast<ASpacecraftPawn>(OtherActor);

		// Acquire a target only if there isn't one at the moment.
		// Target must be part of a different faction to be considered an enemy for this NPC pawn.
		if (NewTarget && NewTarget->IsNotDestroyed() && NewTarget->GetFaction() != this->GetFaction())
		{
			OnNewEnemyFound(NewTarget);
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectExitDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacecraftPawn* LostTarget = Cast<ASpacecraftPawn>(OtherActor);

		// Stop hunting only if the target spacecraft has gone out of the detection area.
		if (LostTarget && Target == LostTarget)
		{
			if (!bAlwaysAggressive)		// aggressive spacecrafts should not stop chasing their target.
			{
				OnEnemyLost();
			}
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectEnterCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacecraftPawn* ClosingTarget = Cast<ASpacecraftPawn>(OtherActor);

		// Make sure this spacecraft keeps attacking the same target.
		if (ClosingTarget && Target == ClosingTarget)
		{
			OnEnemyEnterCombatArea();
		}
	}
}

void ASpaceEnemyPawn::ExecuteOnObjectExitCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor != NULL && OtherActor != this)
	{
		ASpacecraftPawn* FleeingTarget = Cast<ASpacecraftPawn>(OtherActor);

		// Make sure this spacecraft keeps attacking the same target.
		if (FleeingTarget && Target == FleeingTarget)
		{
			OnEnemyExitCombatArea();
		}
	}
}

void ASpaceEnemyPawn::OnTurboModeActivated()
{

}

void ASpaceEnemyPawn::OnTurboModeDeactivated()
{

}

float ASpaceEnemyPawn::ComputeModifiedAreaRadius(float OriginalRadius, float RadiusModifier)
{
	return OriginalRadius + OriginalRadius * RadiusModifier / 100.0f;
}

void ASpaceEnemyPawn::OnMouseEnter_Implementation()
{
	// Show HP/SP widget.
	SurvivabilitySummaryWidgetComponent->SetVisibility(true);

	bCurrentlyPointedAt = true;
}

void ASpaceEnemyPawn::OnMouseLeave_Implementation()
{
	// Hide HP/SP widget.
	SurvivabilitySummaryWidgetComponent->SetVisibility(false);

	bCurrentlyPointedAt = false;
}

void ASpaceEnemyPawn::OnDamageTaken(ASpacecraftPawn* DamageCauser, int32 DamageTaken)
{
	// If this Pawn is not currently pointed at, the widget will be visible only temporarily.
	if (! bCurrentlyPointedAt)
	{
		// Make the HP/SP HUD element (widget) for a limited amount of time whenever taking damage.
		// Getting damaged while the timer is still counting resets it.
	
		// Clear timer.
		GetWorldTimerManager().ClearTimer(ShowSurvivabilityWidgetOnDamageTakenTimerHandle);

		// Show HP/SP widget.
		SurvivabilitySummaryWidgetComponent->SetVisibility(true);

		GetWorldTimerManager().SetTimer(ShowSurvivabilityWidgetOnDamageTakenTimerHandle, [this]() {
			// Do this check again because the value could have changed before the timer had finished counted.
			if (! bCurrentlyPointedAt)
			{
				// Hide HP/SP widget after the timer finished counting.
				SurvivabilitySummaryWidgetComponent->SetVisibility(false);
			}
		}, SurvivabilityWidgetVisibilityTime, false);
	}

	if (Target == nullptr)
	{
		// If this spacecraft is not chasing anyone, follow the one who dared to hit it.

		OnNewEnemyFound(DamageCauser);
	}

	// Display a floating damage widget.
	Cast<ASpaceHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD())->OnSpacecraftTookDamage(GetActorLocation(), DamageTaken, FColor::White);
}

void ASpaceEnemyPawn::PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed)
{
	// ... anything to add here? (besides triggering a loot chest spawn event)

	// Attempt to create loot chests.
	TryToCreateLootChests();

	bShouldPlayDestroyEffects = true;
	bShouldBeDestroyed        = true;
}

void ASpaceEnemyPawn::OnNewEnemyFound(ASpacecraftPawn* NewTarget)
{
	ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
	Target = NewTarget;

	if (SpaceEnemyController)
	{
		DetectionArea->SetSphereRadius(ComputeModifiedAreaRadius(DetectionAreaRadius, DetectionAreaRadiusModifier));

		SpaceEnemyController->NotifyController(ESpaceControllerNotification::TargetFound, Target);
	}
}

void ASpaceEnemyPawn::OnEnemyLost()
{
	ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);
	Target = nullptr;

	if (SpaceEnemyController)
	{
		DetectionArea->SetSphereRadius(DetectionAreaRadius);

		SpaceEnemyController->NotifyController(ESpaceControllerNotification::TargetLost);
	}
}

void ASpaceEnemyPawn::OnEnemyEnterCombatArea()
{
	ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);

	if (SpaceEnemyController)
	{
		CloseQuartersArea->SetSphereRadius(ComputeModifiedAreaRadius(CloseQuartersAreaRadius, CloseQuartersAreaRadiusModifier));

		SpaceEnemyController->NotifyController(ESpaceControllerNotification::TargetInsideCombatArea);
	}
}

void ASpaceEnemyPawn::OnEnemyExitCombatArea()
{
	ASpaceEnemyController* SpaceEnemyController = Cast<ASpaceEnemyController>(Controller);

	if (SpaceEnemyController)
	{
		CloseQuartersArea->SetSphereRadius(CloseQuartersAreaRadius);

		SpaceEnemyController->NotifyController(ESpaceControllerNotification::TargetOutOfCombatArea);
	}
}

void ASpaceEnemyPawn::UpdateStateBasedOnAggressiveness()
{
	float DeterminedDetectionRadius = bAlwaysAggressive
		? ComputeModifiedAreaRadius(DetectionAreaRadius, DetectionAreaRadiusModifier)
		: DetectionAreaRadius;

	DetectionArea->SetSphereRadius(DeterminedDetectionRadius);
}

void ASpaceEnemyPawn::BeginFiringWeapon()
{
	if (Target)
	{
		// Only allow firing when the target is within this spacecraft's minimum attacking distance.
		float MinAttackRange = DetectionArea->GetScaledSphereRadius();

		if (GetDistanceToCurrentTarget() <= MinAttackRange)
		{
			Super::BeginFiringWeapon();
		}
	}
}

void ASpaceEnemyPawn::EndFiringWeapon()
{
	Super::EndFiringWeapon();

}

void ASpaceEnemyPawn::OnWeaponOverheated_Implementation(AWeapon* OverheatedWeapon)
{
	Super::OnWeaponOverheated_Implementation(OverheatedWeapon);

	if (OverheatedWeapon == EquippedWeapon)
	{
		int32 UsableWeaponIndex = FindRandomCooledDownWeaponActiveSlotIndex();

		if (UsableWeaponIndex > 0)
		{
			EquipWeaponFromSlot(UsableWeaponIndex);

			bWaitingForWeaponToCoolDown = false;
		}
		else
		{
			// OnCooledDown will check for this variable and equip the given weapon if true.
			bWaitingForWeaponToCoolDown = true;
		}
	}
}

void ASpaceEnemyPawn::OnWeaponCooledDown_Implementation(AWeapon* CooledDownWeapon)
{
	Super::OnWeaponCooledDown_Implementation(CooledDownWeapon);

	if (bWaitingForWeaponToCoolDown)
	{
		int32 WeaponToUseIndex = FindSlotIndexForWeapon(CooledDownWeapon);

		if (WeaponToUseIndex > 0)
		{
			EquipWeaponFromSlot(WeaponToUseIndex);

			bWaitingForWeaponToCoolDown = false;
		}
	}
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
		FActorSpawnParameters SpawnParams;
		FTransform ChestTransform;
		FVector    ChestLocation = GetActorLocation() + FMath::RandPointInBox(LootBoundingBox);
		FRotator   ChestRotation = GetActorRotation();

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Spawn chest with a random rotation around the Z (up) axis.
		ChestRotation.Yaw += FMath::RandRange(0.0f, 359.9f);

		ChestTransform.SetLocation(ChestLocation);
		ChestTransform.SetRotation(ChestRotation.Quaternion());

		World->SpawnActor<ALootChest>(ChestTypeToSpawn, ChestTransform, SpawnParams);
	}
}

TArray<ASpacecraftPawn*> ASpaceEnemyPawn::GetAllSpacecraftsInDetectionArea(ESpacecraftFaction Factions)
{
	TArray<AActor*> FoundShipsAsActors;
	TArray<ASpacecraftPawn*> FoundShips;

	DetectionArea->GetOverlappingActors(FoundShipsAsActors, ASpacecraftPawn::StaticClass());
	
	for (AActor* ShipActor : FoundShipsAsActors)
	{
		ASpacecraftPawn* CastedShip = Cast<ASpacecraftPawn>(ShipActor);

		if (CastedShip && ((uint8) CastedShip->GetFaction() & (uint8) Factions))
		{
			FoundShips.Add(CastedShip);
		}
	}

	return FoundShips;
}

float ASpaceEnemyPawn::GetDistanceToCurrentTarget()
{
	FVector DistanceToTargetVector = Target->GetActorLocation() - this->GetActorLocation();

	return DistanceToTargetVector.Size();
}

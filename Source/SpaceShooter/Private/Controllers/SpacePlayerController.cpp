// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Controllers/SpacePlayerController.h"
#include "Pawns/SpacePlayerPawn.h"
#include "UI/SpaceHUD.h"
#include "Listeners/MousePointerListener.h"
#include "Loot/Containers/LootChest.h"

#include "Engine/World.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


/** Sets default values. */
ASpacePlayerController::ASpacePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

}

/** Called when the game starts or when spawned. */
void ASpacePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Cast the pawn to our custom player type so we can make use of its public methods.
	PossessedSpacePawn = Cast<ASpacePlayerPawn>(GetPawn());
	PlayerSpacePawnToPossessAux = PossessedSpacePawn;

	// Cast the HUD to our custom HUD type so we can make use of its public methods.
	OwnedHUD = Cast<ASpaceHUD>(GetHUD());
}

/** Called every frame. */
void ASpacePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTargetIconOnScreen();
	HandleSpaceshipRotation();
	HandleCursorPointingAtMouseListeningActors();
}

/** Called to bind functionality to input. */
void ASpacePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Move Forward", this, &ASpacePlayerController::MovePawnForward);
	InputComponent->BindAxis("Move Backward", this, &ASpacePlayerController::MovePawnBackward);

	InputComponent->BindAction("Turbo Mode", IE_Pressed, this, &ASpacePlayerController::ActivateTurboMode);
	InputComponent->BindAction("Turbo Mode", IE_Released, this, &ASpacePlayerController::DeactivateTurboMode);

	InputComponent->BindAction("Fire Weapon", IE_Pressed, this, &ASpacePlayerController::BeginFiringWeapon);
	InputComponent->BindAction("Fire Weapon", IE_Released, this, &ASpacePlayerController::EndFiringWeapon);

	InputComponent->BindAction("Equip Weapon 1", IE_Pressed, this, &ASpacePlayerController::EquipWeapon_1);
	InputComponent->BindAction("Equip Weapon 2", IE_Pressed, this, &ASpacePlayerController::EquipWeapon_2);
	InputComponent->BindAction("Equip Weapon 3", IE_Pressed, this, &ASpacePlayerController::EquipWeapon_3);
	InputComponent->BindAction("Equip Weapon 4", IE_Pressed, this, &ASpacePlayerController::EquipWeapon_4);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ASpacePlayerController::Interact);
	InputComponent->BindAction("Previous Item In Loot Chest", IE_Pressed, this, &ASpacePlayerController::HighlightPreviousItemBoxInsideChest);
	InputComponent->BindAction("Next Item In Loot Chest", IE_Pressed, this, &ASpacePlayerController::HighlightNextItemBoxInsideChest);

	InputComponent->BindAction("Toggle Inventory", IE_Pressed, this, &ASpacePlayerController::ToggleHUDInventory)
		.bExecuteWhenPaused = true;
	InputComponent->BindAction("Toggle In-game Pause Menu", IE_Pressed, this, &ASpacePlayerController::ToggleInGamePauseMenu)
		.bExecuteWhenPaused = true;
}

void ASpacePlayerController::HandleTargetIconOnScreen()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed() && !UGameplayStatics::IsGamePaused(GetWorld()))
	{
		float posX, posY;

		if (GetMousePosition(posX, posY) && OwnedHUD)
		{
			OwnedHUD->UpdateCrosshairIconPosition(posX, posY);
		}
	}
}

/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
void ASpacePlayerController::HandleSpaceshipRotation()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed() && !UGameplayStatics::IsGamePaused(GetWorld()))
	{
		FRotator TargetRotation;
		FHitResult HitResult;
		bool bHitSuccessful;
		
		// TraceTypeQuery3 is defined as "Background" trace channel in the Editor, so we're going to use that.
		bHitSuccessful = GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, HitResult);
		
		if (bHitSuccessful)
		{
			TargetRotation       = (HitResult.ImpactPoint - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
			TargetRotation.Pitch = 0.0f;

			PossessedSpacePawn->RotateSpacecraft(TargetRotation);
		}
	}
}

void ASpacePlayerController::HandleCursorPointingAtMouseListeningActors()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed() && !UGameplayStatics::IsGamePaused(GetWorld()))
	{
		FHitResult HitResult;
		bool bHitSuccessful;
		
		// TraceTypeQuery2 is defined as "Camera" trace channel in the Editor, so we're going to use that.
		bHitSuccessful = GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery2, true, HitResult);
		
		if (bHitSuccessful && HitResult.GetActor())
		{
			if (HitResult.GetActor()->Implements<UMousePointerListener>())
			{
				AActor* FoundActor = HitResult.GetActor();

				// If we were previously pointing to another actor, then call OnMouseLeave for that one.
				if (CurrentMouseListeningActorPointedAt != nullptr && CurrentMouseListeningActorPointedAt != FoundActor)
				{
					IMousePointerListener::Execute_OnMouseLeave(CurrentMouseListeningActorPointedAt);
					CurrentMouseListeningActorPointedAt = nullptr;
				}

				// If we were previously pointing to nothing, it doesn't matter.

				// If we were previously pointing to the same new found actor, it doesn't matter.
				
				// New actor found! Call OnMouseEnter for this one and remember it.
				if (CurrentMouseListeningActorPointedAt == nullptr)
				{
					CurrentMouseListeningActorPointedAt = FoundActor;
					IMousePointerListener::Execute_OnMouseEnter(CurrentMouseListeningActorPointedAt);
				}
			}
			else
			{
				// We found no mouse listening actor, so if we were previously pointing to an actor, we've lost it.
				if (CurrentMouseListeningActorPointedAt != nullptr)
				{
					IMousePointerListener::Execute_OnMouseLeave(CurrentMouseListeningActorPointedAt);
					CurrentMouseListeningActorPointedAt = nullptr;
				}
			}
		}
	}
}

void ASpacePlayerController::MovePawnForward(float Value)
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->MoveForward(Value);
	}
}

void ASpacePlayerController::MovePawnBackward(float Value)
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->MoveBackward(Value);
	}
}

void ASpacePlayerController::ActivateTurboMode()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->ActivateTurboMode();
	}
}

void ASpacePlayerController::DeactivateTurboMode()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->DeactivateTurboMode();
	}
}

void ASpacePlayerController::EquipWeapon_1()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->EquipWeaponFromSlot_1();
	}
}

void ASpacePlayerController::EquipWeapon_2()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->EquipWeaponFromSlot_2();
	}
}

void ASpacePlayerController::EquipWeapon_3()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->EquipWeaponFromSlot_3();
	}
}

void ASpacePlayerController::EquipWeapon_4()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->EquipWeaponFromSlot_4();
	}
}

void ASpacePlayerController::OnPlayerDied()
{
	OwnedHUD->SetCanDrawCrosshairIcon(false);

	bShowMouseCursor = true;
}

void ASpacePlayerController::OnPlayerRespawned()
{
	bShowMouseCursor = false;

	OwnedHUD->SetCanDrawCrosshairIcon(true);
}

void ASpacePlayerController::TogglePawnPossession()
{
	if (PossessedSpacePawn)
		DisablePawnPossession();
	else
		EnablePawnPossession();
}

void ASpacePlayerController::DisablePawnPossession()
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->EndFiringWeapon();
		PossessedSpacePawn->DeactivateTurboMode();

		PossessedSpacePawn = nullptr;
	}
}

void ASpacePlayerController::EnablePawnPossession()
{
	PossessedSpacePawn = PlayerSpacePawnToPossessAux;
}

void ASpacePlayerController::Interact()
{
	if (!PossessedSpacePawn || !PossessedSpacePawn->CanInteract())
		return;

	// If we're currently pointing at an object...
	if (CurrentMouseListeningActorPointedAt != nullptr)
	{
		// ... and if it is a chest, interact with it.
		ALootChest* FoundLootChest = Cast<ALootChest>(CurrentMouseListeningActorPointedAt);
		if (FoundLootChest)
		{
			FoundLootChest->Interact(PossessedSpacePawn);
		}
	}
	// Else, consider this interaction as an attempt to quickly collect items from nearby loot chests,
	//	like a vacuum cleaner.
	else
	{
		PossessedSpacePawn->SearchAndCollectNearbySupplies();
	}
}

void ASpacePlayerController::HighlightPreviousItemBoxInsideChest()
{
	if (CurrentMouseListeningActorPointedAt != nullptr)
	{
		ALootChest* FoundLootChest = Cast<ALootChest>(CurrentMouseListeningActorPointedAt);
		if (FoundLootChest)
		{
			FoundLootChest->HighlightPreviousItemBoxInsideChest();
		}
	}
}

void ASpacePlayerController::HighlightNextItemBoxInsideChest()
{
	if (CurrentMouseListeningActorPointedAt != nullptr)
	{
		ALootChest* FoundLootChest = Cast<ALootChest>(CurrentMouseListeningActorPointedAt);
		if (FoundLootChest)
		{
			FoundLootChest->HighlightNextItemBoxInsideChest();
		}
	}
}

void ASpacePlayerController::BeginFiringWeapon()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->BeginFiringWeapon();
	}
}

void ASpacePlayerController::EndFiringWeapon()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		PossessedSpacePawn->EndFiringWeapon();
	}
}

void ASpacePlayerController::ToggleHUDInventory()
{
	if (PlayerSpacePawnToPossessAux && PlayerSpacePawnToPossessAux->IsNotDestroyed() && OwnedHUD)
	{
		OwnedHUD->ToggleInventoryInterface();
	}
}

void ASpacePlayerController::ToggleInGamePauseMenu()
{
	if (PlayerSpacePawnToPossessAux && PlayerSpacePawnToPossessAux->IsNotDestroyed() && OwnedHUD)
	{
		OwnedHUD->ToggleInGamePauseMenuInterface();
	}
}

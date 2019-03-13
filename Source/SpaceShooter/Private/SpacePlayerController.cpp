// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacePlayerController.h"
#include "SpacePlayerPawn.h"
#include "SpaceHUD.h"


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

	// Cast the HUD to our custom HUD type so we can make use of its public methods.
	OwnedHUD = Cast<ASpaceHUD>(GetHUD());
}

/** Called every frame. */
void ASpacePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTargetIconOnScreen();
	HandleSpaceshipRotation();
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
}

void ASpacePlayerController::HandleTargetIconOnScreen()
{
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
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
	if (PossessedSpacePawn && PossessedSpacePawn->IsNotDestroyed())
	{
		FRotator targetRotation;
		FHitResult hitResult;
		bool bHitSuccessful;
		
		// TraceTypeQuery3 is defined as Background trace channel in the Editor, so we're going to use that.
		bHitSuccessful = GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery3, true, hitResult);
		
		if (bHitSuccessful)
		{
			targetRotation       = (hitResult.ImpactPoint - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
			targetRotation.Pitch = 0.0f;

			PossessedSpacePawn->RotateSpacecraft(targetRotation);
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

void ASpacePlayerController::SignalPlayerDied()
{
	OwnedHUD->SetCanDrawCrosshairIcon(false);

	bShowMouseCursor = true;
}

void ASpacePlayerController::SignalPlayerRespawned()
{
	bShowMouseCursor = false;

	OwnedHUD->SetCanDrawCrosshairIcon(true);
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

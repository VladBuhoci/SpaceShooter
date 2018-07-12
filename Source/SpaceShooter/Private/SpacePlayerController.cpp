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

	HandlePlayerTargetIconOnScreen();
	HandlePlayerSpaceshipRotation();
}

/** Called to bind functionality to input. */
void ASpacePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Move Forward", this, &ASpacePlayerController::MovePawnForward);
	InputComponent->BindAxis("Move Backward", this, &ASpacePlayerController::MovePawnBackward);
}

void ASpacePlayerController::HandlePlayerTargetIconOnScreen()
{
	float posX, posY;

	if (GetMousePosition(posX, posY) && OwnedHUD)
	{
		OwnedHUD->UpdateTargetImagePosition(posX, posY);
	}
}

/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
void ASpacePlayerController::HandlePlayerSpaceshipRotation()
{
	if (PossessedSpacePawn)
	{
		FRotator targetRotation;
		TArray<TEnumAsByte<EObjectTypeQuery>> objectTypeArray({ EObjectTypeQuery::ObjectTypeQuery1 }, 1);	// ObjectTypeQuery1 = WorldStatic
		FHitResult hitResult;
		bool bHitSuccessful;
		
		bHitSuccessful = GetHitResultUnderCursorForObjects(objectTypeArray, true, hitResult);

		if (bHitSuccessful)
		{
			targetRotation       = (hitResult.ImpactPoint - PossessedSpacePawn->GetActorLocation()).ToOrientationRotator();
			targetRotation.Pitch = 0.0f;

			PossessedSpacePawn->RotateSpaceship(targetRotation);
		}
	}
}

void ASpacePlayerController::MovePawnForward(float Value)
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->MoveForward(Value);
	}
}

void ASpacePlayerController::MovePawnBackward(float Value)
{
	if (PossessedSpacePawn)
	{
		PossessedSpacePawn->MoveBackward(Value);
	}
}

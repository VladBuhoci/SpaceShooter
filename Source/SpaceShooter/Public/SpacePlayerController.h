// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpacePlayerController.generated.h"

// Forward declarations.
class ASpacePlayerPawn;
class ASpaceHUD;


/**
 * Space Player Pawn Controller.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	/** The controlled player pawn of this controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player Controller", Meta = (AllowPrivateAccess = "true"))
	ASpacePlayerPawn * PossessedSpacePawn;

	/** The HUD instance this controller owns. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player Controller", Meta = (AllowPrivateAccess = "true"))
	ASpaceHUD * OwnedHUD;

public:
	/** Sets default values. */
	ASpacePlayerController();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/** Called to bind functionality to input. */
	virtual void SetupInputComponent() override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Sends a request to the spaceship to activate its primary weapons. */
	void BeginFiringPrimaryWeapons();

	/** Sends a request to the spaceship to deactivate its primary weapons. */
	void EndFiringPrimaryWeapons();


	/**********************************
		   PLAYER INPUT HANDLING
	**********************************/

private:
	/** Updates the target icon according to the current position of the user's cursor on screen. */
	void HandleTargetIconOnScreen();
	
	/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
	void HandleSpaceshipRotation();

	void MovePawnForward(float Value);
	void MovePawnBackward(float Value);

	void ActivateTurboMode();
	void DeactivateTurboMode();
};

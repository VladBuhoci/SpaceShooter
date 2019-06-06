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

	/** The mouse listening actor object the player is having their cursor put on at the moment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player Controller", Meta = (AllowPrivateAccess = "true"))
	AActor* CurrentMouseListeningActorPointedAt;

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
	void BeginFiringWeapon();

	/** Sends a request to the spaceship to deactivate its primary weapons. */
	void EndFiringWeapon();


	/**********************************
		   PLAYER INPUT HANDLING
	**********************************/

private:
	/** Updates the target icon according to the current position of the user's cursor on screen. */
	void HandleTargetIconOnScreen();
	
	/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
	void HandleSpaceshipRotation();

	/**
	 * Gets the position of the cursor on the game screen and triggers a mouse event (enter/leave) if
	 *		the found actor implements the IMousePointerListener interface.
	 */
	void HandleCursorPointingAtMouseListeningActors();

	void MovePawnForward(float Value);
	void MovePawnBackward(float Value);

	void ActivateTurboMode();
	void DeactivateTurboMode();

	void EquipWeapon_1();
	void EquipWeapon_2();
	void EquipWeapon_3();
	void EquipWeapon_4();


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

public:
	void OnPlayerDied();
	void OnPlayerRespawned();


	/**********************************
			  LOOT INTERFACE
	**********************************/

public:
	/** Get the mouse listening actor that the player is pointing at with the mouse cursor. */
	UFUNCTION(BlueprintPure, Category = "Space Player Controller")
	AActor* GetCurrentMouseListeningActorPointedAt() const { return CurrentMouseListeningActorPointedAt; }

private:
	void Interact();
	void HighlightPreviousItemBoxInsideChest();
	void HighlightNextItemBoxInsideChest();


	/**********************************
			   HUD INTERFACE
	**********************************/

private:
	void ToggleHUDInventory();
	void ToggleInGamePauseMenu();
};

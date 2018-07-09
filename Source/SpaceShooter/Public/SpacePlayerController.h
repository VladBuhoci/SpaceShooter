// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpacePlayerController.generated.h"

/**
 * Space Player Pawn controller.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	/** The controlled player pawn of this controller. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player Controller", Meta = (AllowPrivateAccess = "true"))
	class ASpacePlayerPawn * PossessedSpacePawn;

	/** The HUD instance this controller owns. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Player Controller", Meta = (AllowPrivateAccess = "true"))
	class ASpaceHUD * OwnedHUD;

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

private:
	/** Updates the target icon according to the current position of the user's cursor on screen. */
	void HandlePlayerTargetIconOnScreen();
	
	/** Gets the position of the cursor on the game screen and asks the player to rotate towards it. */
	void HandlePlayerSpaceshipRotation();

	// Player pawn input handling:
	void MovePawnForward(float Value);
	void MovePawnBackward(float Value);
	// ~ end of player pawn input handling.
};

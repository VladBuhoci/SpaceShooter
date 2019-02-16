// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SpaceHUD.generated.h"


USTRUCT(BlueprintInternalUseOnly)
struct FCrosshairParams
{
	GENERATED_USTRUCT_BODY()

	/** The crosshair image to be rendered on screen where the mouse cursor points at. */
	UMaterialInterface* Icon;
	
	/** Crosshair image X coordinate. */
	float PosX;

	/** Crosshair image Y coordinate. */
	float PosY;
};

/**
 * The Space Game Mode's HUD class.
 */
UCLASS()
class SPACESHOOTER_API ASpaceHUD : public AHUD
{
	GENERATED_BODY()
	
	/** Controls the end result of the crosshair's rendering every frame. */
	FCrosshairParams CrosshairParams;

	/** True if the crosshair icon should be currently drawn on screen. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	bool bCanDrawCrosshair;

public:
	ASpaceHUD();

	/** The Main Draw loop for the HUD. */
	virtual void DrawHUD() override;

	// Public interface:
public:
	/** Update the crosshair icon's coordinates on the screen. */
	void UpdateCrosshairIconPosition(float newPosX, float newPosY);

	void SetCanDrawCrosshairIcon(bool NewState) { bCanDrawCrosshair = NewState; }

	// ~ end of public interface.
};

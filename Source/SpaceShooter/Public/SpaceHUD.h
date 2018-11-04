// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SpaceHUD.generated.h"

/**
 * The Space Game Mode's HUD class.
 */
UCLASS()
class SPACESHOOTER_API ASpaceHUD : public AHUD
{
	GENERATED_BODY()
	
	/** The target image to be rendered on screen where the mouse cursor points at. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	UMaterialInterface * TargetIcon;
	
	/** Target image X coordinate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	float TargetPosX;

	/** Target image Y coordinate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	float TargetPosY;

	/** True if the target icon should be currently drawn on screen. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	bool bCanDrawTarget;

public:
	ASpaceHUD();

	/** The Main Draw loop for the HUD. */
	virtual void DrawHUD() override;

	// Public interface:
public:
	/** Redraws the target image on the screen using the new coordinates. */
	void UpdateTargetIconPosition(float newPosX, float newPosY);

	void SetCanDrawTargetIcon(bool NewState) { bCanDrawTarget = NewState; }

	// ~ end of public interface.
};

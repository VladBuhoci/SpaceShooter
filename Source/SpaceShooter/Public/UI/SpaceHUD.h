// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SpaceHUD.generated.h"

// Forward declarations.
class ASpacePlayerController;

class UUserWidget;

enum class ESlateVisibility : uint8;


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
	
	/** Store the player controller so we don't look for it all the time. */
	ASpacePlayerController* SpacePlayerController;

	/** Controls the end result of the crosshair's rendering every frame. */
	FCrosshairParams CrosshairParams;

	/** True if the crosshair icon should be currently drawn on screen. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	bool bCanDrawCrosshair;

	/** Class of the widget containing all the core UI elements for the selected game mode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> AllInOneGameHUDWidgetType;

	/** Type of inventory widget to spawn and add it on screen when requested. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InventoryWidgetType;

	/** Type of in-game menu widget to spawn and add it on screen when requested. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> InGamePauseMenuWidgetType;

	/** 
	 * Type of in-game overall statistics menu widget to spawn when completing a level or when destroyed
	 *	and add it on screen when requested.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> GameEndStatsMenuWidgetType;

	/** Array of widgets that should only be visible during game time and not when a pausing menu is opened. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	TArray<UUserWidget*> GameTimeWidgets;

protected:
	/** Pointer to the all-in-one-hud widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* AllInOneGameHUDWidget;

	/** Pointer to the inventory widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* InventoryWidget;

	/** Pointer to the in-game menu widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* InGamePauseMenuWidget;

	/** Pointer to the in-game overall statistics menu widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* GameEndStatsMenuWidget;

	bool bShowingLevelEndStatsWidget;

public:
	/** Sets default values. */
	ASpaceHUD();

	/** The Main Draw loop for the HUD. */
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

	// Public interface:
public:
	/** Update the crosshair icon's coordinates on the screen. */
	void UpdateCrosshairIconPosition(float newPosX, float newPosY);

	void SetCanDrawCrosshairIcon(bool NewState) { bCanDrawCrosshair = NewState; }

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Space HUD")
	void OnPlayerSpacecraftActiveSlotsStateChanged();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Space HUD")
	void OnPlayerSpacecraftInventoryStateChanged();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Space HUD")
	void OnInventoryInterfaceVisibilityChanged(ESlateVisibility NewState);

	UFUNCTION(BlueprintCallable, Category = "Space HUD")
	void ToggleInventoryInterface();

	UFUNCTION(BlueprintCallable, Category = "Space HUD")
	void ToggleInGamePauseMenuInterface();

	UFUNCTION(BlueprintCallable, Category = "Space HUD")
	void ToggleGameEndStatsMenuInterface();

	// ~ end of public interface.

protected:
	void ToggleLevelEndStatsMenuInterface(UUserWidget* LevelEndStatsWidget);
	void ToggleCursorVisibility(bool bInputUIOnly);

	virtual void CreateAndAddWidgets(TArray<UUserWidget*> & GameTimeVisibleWidgets);

	void TryCreateAndAddWidget(TSubclassOf<UUserWidget> WidgetClass, UUserWidget* & Widget, ESlateVisibility Visbility);

	ASpacePlayerController* GetSpacePlayerController() const { return SpacePlayerController; }
	ESlateVisibility GetWidgetOppositeVisibilityState(UUserWidget* Widget) const;

private:
	void ToggleGameTimeWidgetsVisibility(ESlateVisibility NewState);
};

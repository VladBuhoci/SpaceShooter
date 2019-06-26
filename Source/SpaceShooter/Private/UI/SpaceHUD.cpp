// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "UI/SpaceHUD.h"
#include "Controllers/SpacePlayerController.h"

#include "ConstructorHelpers.h"

#include "Materials/MaterialInterface.h"

#include "UserWidget.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


ASpaceHUD::ASpaceHUD()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> CrosshairIconFinder(TEXT("Material'/Game/Materials/HUD/Crosshairs/M_Crosshair_01.M_Crosshair_01'"));
	
	if (CrosshairIconFinder.Succeeded())
	{
		CrosshairParams.Icon = CrosshairIconFinder.Object;
	}

	CrosshairParams.PosX = 0.0f;
	CrosshairParams.PosY = 0.0f;
	bCanDrawCrosshair    = true;
}

void ASpaceHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* WorldPtr = GetWorld();
	if (WorldPtr)
	{
		SpacePlayerController = Cast<ASpacePlayerController>(PlayerOwner);
	}

	CreateAndAddWidgets(GameTimeWidgets);
}

void ASpaceHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bCanDrawCrosshair)
	{
		if (CrosshairParams.Icon)
		{
			DrawMaterialSimple(CrosshairParams.Icon, CrosshairParams.PosX - 16, CrosshairParams.PosY - 16, 32, 32);
		}
	}
}

void ASpaceHUD::UpdateCrosshairIconPosition(float newPosX, float newPosY)
{
	CrosshairParams.PosX = newPosX;
	CrosshairParams.PosY = newPosY;
}

void ASpaceHUD::ToggleInventoryInterface()
{
	if (InventoryWidget && !bShowingLevelEndStatsWidget && InGamePauseMenuWidget->GetVisibility() != ESlateVisibility::Visible)
	{
		// Disable controls of the player's spacecraft pawn.
		if (SpacePlayerController)
		{
			SpacePlayerController->TogglePawnPossession();
		}

		ESlateVisibility NewVisibilityState = GetWidgetOppositeVisibilityState(InventoryWidget);
		bool bGamePaused = !UGameplayStatics::IsGamePaused(GetWorld());

		ToggleGameTimeWidgetsVisibility(InventoryWidget->GetVisibility());
		ToggleCursorVisibility(bGamePaused);

		InventoryWidget->SetVisibility(NewVisibilityState);
		InGamePauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

		// Also start/stop time.
		UGameplayStatics::SetGamePaused(GetWorld(), bGamePaused);
	}
}

void ASpaceHUD::ToggleInGamePauseMenuInterface()
{
	if (InGamePauseMenuWidget && !bShowingLevelEndStatsWidget && InventoryWidget->GetVisibility() != ESlateVisibility::Visible)
	{
		// Disable controls of the player's spacecraft pawn.
		if (SpacePlayerController)
		{
			SpacePlayerController->TogglePawnPossession();
		}

		ESlateVisibility NewVisiblityState = GetWidgetOppositeVisibilityState(InGamePauseMenuWidget);
		bool bGamePaused = !UGameplayStatics::IsGamePaused(GetWorld());
		
		ToggleGameTimeWidgetsVisibility(InGamePauseMenuWidget->GetVisibility());
		ToggleCursorVisibility(bGamePaused);

		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		InGamePauseMenuWidget->SetVisibility(NewVisiblityState);

		// Also start/stop time.
		UGameplayStatics::SetGamePaused(GetWorld(), bGamePaused);
	}
}

void ASpaceHUD::ToggleGameEndStatsMenuInterface()
{
	if (GameEndStatsMenuWidget)
	{
		ToggleLevelEndStatsMenuInterface(GameEndStatsMenuWidget);
	}
}

void ASpaceHUD::ToggleLevelEndStatsMenuInterface(UUserWidget* LevelEndStatsWidget)
{
	if (!bShowingLevelEndStatsWidget)
	{
		bShowingLevelEndStatsWidget = true;

		// Disable controls of the player's spacecraft pawn.
		if (SpacePlayerController)
		{
			SpacePlayerController->TogglePawnPossession();
		}

		ESlateVisibility NewVisiblityState = GetWidgetOppositeVisibilityState(LevelEndStatsWidget);

		ToggleGameTimeWidgetsVisibility(LevelEndStatsWidget->GetVisibility());
		ToggleCursorVisibility(true);

		InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
		InGamePauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		LevelEndStatsWidget->SetVisibility(NewVisiblityState);
	}
}

void ASpaceHUD::CreateAndAddWidgets(TArray<UUserWidget*> & GameTimeVisibleWidgets)
{
	if (!SpacePlayerController)
		return;

	SpacePlayerController->SetInputMode(FInputModeGameOnly());
	
	TryCreateAndAddWidget(AllInOneGameHUDWidgetType, AllInOneGameHUDWidget, ESlateVisibility::Visible);
	TryCreateAndAddWidget(InventoryWidgetType, InventoryWidget, ESlateVisibility::Collapsed);
	TryCreateAndAddWidget(InGamePauseMenuWidgetType, InGamePauseMenuWidget, ESlateVisibility::Collapsed);
	TryCreateAndAddWidget(GameEndStatsMenuWidgetType, GameEndStatsMenuWidget, ESlateVisibility::Collapsed);

	GameTimeVisibleWidgets.Add(AllInOneGameHUDWidget);
}

void ASpaceHUD::TryCreateAndAddWidget(TSubclassOf<UUserWidget> WidgetClass, UUserWidget* & Widget, ESlateVisibility Visbility)
{
	if (WidgetClass)
	{
		Widget = CreateWidget<UUserWidget>(SpacePlayerController, WidgetClass);
		if (Widget)
		{
			Widget->AddToViewport();
			Widget->SetVisibility(Visbility);
		}
	}
}

ESlateVisibility ASpaceHUD::GetWidgetOppositeVisibilityState(UUserWidget* Widget) const
{
	return Widget->GetVisibility() == ESlateVisibility::Visible
		? ESlateVisibility::Collapsed
		: ESlateVisibility::Visible;
}

void ASpaceHUD::ToggleGameTimeWidgetsVisibility(ESlateVisibility NewState)
{
	for (auto GameOnlyVisibleWidget : GameTimeWidgets)
	{
		GameOnlyVisibleWidget->SetVisibility(NewState);
	}
}

void ASpaceHUD::ToggleCursorVisibility(bool bInputGameAndUI)
{
	if (!SpacePlayerController)
		return;

	UGameViewportClient* GameViewport = GetWorld()->GetGameViewport();
	if (GameViewport)
	{
		if (bInputGameAndUI)
		{
			SpacePlayerController->SetInputMode(FInputModeGameAndUI());

			GameViewport->SetHideCursorDuringCapture(false);
		}
		else
		{
			SpacePlayerController->SetInputMode(FInputModeGameOnly());

			GameViewport->SetMouseLockMode(EMouseLockMode::DoNotLock);
		}
	}
	
	SpacePlayerController->bShowMouseCursor   = bInputGameAndUI;
	SpacePlayerController->bEnableClickEvents = bInputGameAndUI;
	
	SetCanDrawCrosshairIcon(!bInputGameAndUI);
}

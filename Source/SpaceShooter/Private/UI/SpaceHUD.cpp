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
		SpacePlayerController = Cast<ASpacePlayerController>(UGameplayStatics::GetPlayerController(WorldPtr, 0));
	}

	CreateAndAddWidgets();
}

void ASpaceHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bCanDrawCrosshair)
	{
		if (CrosshairParams.Icon)
		{
			// TODO: Is there any way to determine the width and height programmatically?
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
	if (InventoryWidget && !bShowingLevelEndStatsWidget)
	{
		ESlateVisibility NewVisibilityState = InventoryWidget->GetVisibility() == ESlateVisibility::Visible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		InventoryWidget->SetVisibility(NewVisibilityState);
	}
}

void ASpaceHUD::ToggleInGamePauseMenuInterface()
{
	if (InGamePauseMenuWidget && SpacePlayerController && !bShowingLevelEndStatsWidget)
	{
		ESlateVisibility NewVisiblityState = InGamePauseMenuWidget->GetVisibility() == ESlateVisibility::Visible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		InGamePauseMenuWidget->SetVisibility(NewVisiblityState);

		bool bGamePaused = ! UGameplayStatics::IsGamePaused(GetWorld());
		
		ToggleCursorVisibility(bGamePaused);

		// Also start/stop time.
		UGameplayStatics::SetGamePaused(GetWorld(), bGamePaused);
	}
}

void ASpaceHUD::ToggleLevelEndStatsMenuInterface()
{
	if (LevelEndStatsMenuWidget && !bShowingLevelEndStatsWidget)
	{
		bShowingLevelEndStatsWidget = true;

		ESlateVisibility NewVisiblityState = LevelEndStatsMenuWidget->GetVisibility() == ESlateVisibility::Visible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		LevelEndStatsMenuWidget->SetVisibility(NewVisiblityState);

		ToggleCursorVisibility(true);
	}
}

void ASpaceHUD::CreateAndAddWidgets()
{
	if (!SpacePlayerController)
		return;

	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		if (InventoryWidgetType)
		{
			InventoryWidget = CreateWidget<UUserWidget>(SpacePlayerController, InventoryWidgetType);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport(100);
				InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (InGamePauseMenuWidgetType)
		{
			InGamePauseMenuWidget = CreateWidget<UUserWidget>(SpacePlayerController, InGamePauseMenuWidgetType);
			if (InGamePauseMenuWidget)
			{
				InGamePauseMenuWidget->AddToViewport(101);
				InGamePauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (LevelEndStatsMenuWidgetType)
		{
			LevelEndStatsMenuWidget = CreateWidget<UUserWidget>(SpacePlayerController, LevelEndStatsMenuWidgetType);
			if (LevelEndStatsMenuWidget)
			{
				LevelEndStatsMenuWidget->AddToViewport(102);
				LevelEndStatsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
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

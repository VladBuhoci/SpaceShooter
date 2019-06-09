// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceHUD.h"
#include "SpacePlayerController.h"

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
	if (InventoryWidget)
	{
		ESlateVisibility NewState = InventoryWidget->GetVisibility() == ESlateVisibility::Visible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		InventoryWidget->SetVisibility(NewState);
	}
}

void ASpaceHUD::ToggleInGamePauseMenuInterface()
{
	if (InGamePauseMenuWidget && SpacePlayerController)
	{
		ESlateVisibility NewState = InGamePauseMenuWidget->GetVisibility() == ESlateVisibility::Visible
			? ESlateVisibility::Collapsed
			: ESlateVisibility::Visible;

		bool NewBooleanState = ! UGameplayStatics::IsGamePaused(GetWorld());
		FInputModeDataBase* InputMode;
		FInputModeUIOnly InputUIOnly;
		FInputModeGameOnly InputGameOnly;

		if (NewBooleanState)
			InputMode = &InputUIOnly;
		else
			InputMode = &InputGameOnly;

		InGamePauseMenuWidget->SetVisibility(NewState);

		// Also start/stop time and show/hide cursor.

		UGameplayStatics::SetGamePaused(GetWorld(), NewBooleanState);
		
		SpacePlayerController->bShowMouseCursor = NewBooleanState;
		SpacePlayerController->bEnableClickEvents = NewBooleanState;
		SpacePlayerController->SetInputMode(*InputMode);
		SetCanDrawCrosshairIcon(! NewBooleanState);
	}
}

void ASpaceHUD::CreateAndAddWidgets()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		if (InGamePauseMenuWidgetType && SpacePlayerController)
		{
			InventoryWidget = CreateWidget<UUserWidget>(SpacePlayerController, InventoryWidgetType);
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport(100);
				InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (InGamePauseMenuWidgetType && SpacePlayerController)
		{
			InGamePauseMenuWidget = CreateWidget<UUserWidget>(SpacePlayerController, InGamePauseMenuWidgetType);
			if (InGamePauseMenuWidget)
			{
				InGamePauseMenuWidget->AddToViewport(100);
				InGamePauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

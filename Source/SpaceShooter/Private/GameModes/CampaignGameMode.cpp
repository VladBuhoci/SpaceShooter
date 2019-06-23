// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/CampaignGameMode.h"
#include "UI/SpaceHUD.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"


ACampaignGameMode::ACampaignGameMode()
{
	GameOverHUDToggleWait = 1.5f;
}

void ACampaignGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ACampaignGameMode::OnSectorCleared()
{
	Super::OnSectorCleared();

	OnCampaignCompleted();
}

void ACampaignGameMode::OnPlayerDestroyed()
{
	Super::OnPlayerDestroyed();

	// Present a basic menu widget (with options like "Restart", "Back to Main Menu" etc.) to the end user.

	UWorld* WorldPtr = GetWorld();
	if (WorldPtr)
	{
		FTimerHandle ToggleGameOverHUDTimerHandle;

		GetWorldTimerManager().SetTimer(ToggleGameOverHUDTimerHandle, [WorldPtr]() {
			ASpaceHUD* SpaceHUD = Cast<ASpaceHUD>(UGameplayStatics::GetPlayerController(WorldPtr, 0)->GetHUD());

			if (SpaceHUD)
			{
				SpaceHUD->ToggleInGamePauseMenuInterface();
			}
		}, GameOverHUDToggleWait, false);
	}
}

void ACampaignGameMode::OnCampaignCompleted()
{
	// Present a widget filled with general statistics related to the player's performance
	//	during the current and previous levels.

	UWorld* WorldPtr = GetWorld();
	if (WorldPtr)
	{
		FTimerHandle ToggleStatsHUDTimerHandle;

		GetWorldTimerManager().SetTimer(ToggleStatsHUDTimerHandle, [WorldPtr]() {
			ASpaceHUD* SpaceHUD = Cast<ASpaceHUD>(UGameplayStatics::GetPlayerController(WorldPtr, 0)->GetHUD());

			if (SpaceHUD)
			{
				SpaceHUD->ToggleLevelEndStatsMenuInterface();
			}
		}, GameOverHUDToggleWait, false);
	}
}

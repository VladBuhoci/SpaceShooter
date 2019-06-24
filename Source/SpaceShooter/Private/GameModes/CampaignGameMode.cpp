// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/CampaignGameMode.h"
#include "Globals/SpaceGameInstance.h"
#include "Pawns/SpacePlayerPawn.h"
#include "UI/CampaignHUD.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"


ACampaignGameMode::ACampaignGameMode()
{
	LevelEndStatsHUDToggleWait = 1.5f;
}

void ACampaignGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* WorldPtr = GetWorld();
	if (WorldPtr)
	{
		CampaignHUD = Cast<ACampaignHUD>(UGameplayStatics::GetPlayerController(WorldPtr, 0)->GetHUD());
	}
}

void ACampaignGameMode::OnEnemySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft)
{
	// TODO
}

void ACampaignGameMode::OnEnemySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft)
{
	EnemiesKilledCount = FMath::Clamp(EnemiesKilledCount + 1, 0, EnemiesToKillTotal);

	CheckIfChapterIsDone();
}

void ACampaignGameMode::OnSectorCleared()
{
	Super::OnSectorCleared();

	CheckIfChapterIsDone();
}

void ACampaignGameMode::OnPlayerDestroyed()
{
	Super::OnPlayerDestroyed();

	// TODO: anything else to do here?

	OnChapterCompleted();
}

void ACampaignGameMode::OnChapterCompleted()
{
	// Present a widget filled with general statistics related to the player's performance during the current level
	//	and a basic menu (with options like "Restart Chapter", "Back to Main Menu" etc.).

	FTimerHandle ToggleStatsHUDTimerHandle;

	GetWorldTimerManager().SetTimer(ToggleStatsHUDTimerHandle, [this]() {
		if (CampaignHUD)
		{
			CampaignHUD->ToggleChapterEndStatsMenuInterface();
		}
	}, LevelEndStatsHUDToggleWait, false);
}

void ACampaignGameMode::OnCampaignCompleted()
{
	// Present a widget filled with general statistics related to the player's performance during the current
	//	and previous levels and a basic menu (with options like "Restart Mission", "Back to Main Menu" etc.).

	FTimerHandle ToggleStatsHUDTimerHandle;

	GetWorldTimerManager().SetTimer(ToggleStatsHUDTimerHandle, [this]() {
		if (CampaignHUD)
		{
			CampaignHUD->ToggleGameEndStatsMenuInterface();
		}
	}, LevelEndStatsHUDToggleWait, false);
}

void ACampaignGameMode::CheckIfChapterIsDone()
{
	if (EnemiesKilledCount == EnemiesToKillTotal)
	{
		USpaceGameInstance* SpaceGameInstance = Cast<USpaceGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		
		if (SpaceGameInstance)
		{
			// Disable player's interaction ability.
			ASpacePlayerPawn* SpacePlayerPawn = Cast<ASpacePlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
			if (SpacePlayerPawn)
			{
				SpacePlayerPawn->SetCanInteract(false);
			}

			if (SpaceGameInstance->GetNextChapterForCurrentMission())
			{
				OnChapterCompleted();
			}
			else
			{
				OnCampaignCompleted();
			}
		}
	}
}

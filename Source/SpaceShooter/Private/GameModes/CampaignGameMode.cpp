// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/CampaignGameMode.h"
#include "GameModes/Gameplay/GoalDescription.h"
#include "Globals/SpaceGameInstance.h"
#include "Pawns/SpacePlayerPawn.h"
#include "UI/CampaignHUD.h"
#include "Controllers/SpacePlayerController.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "AI/Navigation/NavigationSystem.h"


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
		SpaceGameInstance = Cast<USpaceGameInstance>(UGameplayStatics::GetGameInstance(WorldPtr));
		CampaignHUD       = Cast<ACampaignHUD>(UGameplayStatics::GetPlayerController(WorldPtr, 0)->GetHUD());
		NavSystem         = WorldPtr->GetNavigationSystem();
		PlayerPawn        = Cast<ASpacePlayerPawn>(UGameplayStatics::GetPlayerPawn(WorldPtr, 0));
	}

	RetrieveChapterGoal();

	if (GoalToComplete)
	{
		// Store amount of spawned NPCs at game start (minus the player).
		GoalToComplete->NpcSpawnRules.NPCsSpawnedCurrentCount = AllSpacecrafts.Num() - 1;
		// Total amount of NPCs that shall be spawned include already spawned NPCs (put in the level by a map designer).
		GoalToComplete->NpcSpawnRules.TotalNPCsToBeSpawned += GoalToComplete->NpcSpawnRules.NPCsSpawnedCurrentCount;
	}

	if (IsOnlyPlayerLeft())
	{
		OnSectorCleared();
	}
}

void ACampaignGameMode::OnEnemySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft)
{
	// TODO
}

void ACampaignGameMode::OnEnemySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft)
{
	EnemiesKilledCount++;

	if (GoalToComplete)
	{
		GoalToComplete->UpdateGoal(EGoalUpdateType::EnemyDestroyed);
	}
}

void ACampaignGameMode::OnSectorCleared()
{
	Super::OnSectorCleared();

	// If, after clearing the previous group of enemies, we still did not destroy as many ships as the goal's
	//	set of NPC spawn rules specifies, spawn another group.
	// Also, in case the total number of NPCs to be spawned is negative, then we're spawning waves forever.
	if (GoalToComplete
		&& (EnemiesKilledCount < GoalToComplete->NpcSpawnRules.TotalNPCsToBeSpawned
			|| GoalToComplete->NpcSpawnRules.HasInfiniteWaves()))
	{
		CancelUpcomingEnemyWave();	// make sure there aren't any accidentally set up timers that trigger NPC spawning.
		ScheduleNewEnemyWave(GoalToComplete->NpcSpawnRules.bStartAggressive);
	}
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

void ACampaignGameMode::NotifyGoalCompleted_Implementation()
{
	OnChapterObjectiveDone();
}

void ACampaignGameMode::RetrieveChapterGoal()
{
	// Acquire the current chapter's goal and register to it as a listener so we know when it is completed.
	// Also send the goal's unique widget class to the HUD entity so that it can instantiate it and add it to the viewport.
	if (SpaceGameInstance && SpaceGameInstance->GetCurrentChapter())
	{
		UGoalDescription* OriginalGoal = Cast<UGoalDescription>(SpaceGameInstance->GetCurrentChapter()->GetGameplaySpecification());

		if (OriginalGoal)
		{
			FName GoalCloneName = MakeUniqueObjectName(this, OriginalGoal->GetClass());

			GoalToComplete = DuplicateObject(OriginalGoal, this, GoalCloneName);
			GoalToComplete->SetWidgetClass(OriginalGoal->GetWidgetClass());
			GoalToComplete->RegisterListener(this);

			if (CampaignHUD)
			{
				CampaignHUD->SetGoalWidget(GoalToComplete->GetWidgetClass());
			}
		}
	}
}

void ACampaignGameMode::OnChapterObjectiveDone()
{
	if (SpaceGameInstance)
	{
		// Disable player's interaction ability.
		if (PlayerPawn)
		{
			PlayerPawn->SetCanInteract(false);
		}
		
		// Disable player input.
		Cast<ASpacePlayerController>(PlayerPawn->GetController())->DisablePawnPossession();

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

void ACampaignGameMode::CancelUpcomingEnemyWave()
{
	GetWorldTimerManager().ClearTimer(EnemyWaveSpawnTimerHandle);
}

void ACampaignGameMode::ScheduleNewEnemyWave(bool bAggressiveGroup)
{
	float TimeUntilNextWave = GoalToComplete->NpcSpawnRules.GetTimeUntilNextWaveRand();

	GetWorldTimerManager().SetTimer(EnemyWaveSpawnTimerHandle, [this]() {
		SpawnEnemyWave(false);
	}, TimeUntilNextWave, false);
}

void ACampaignGameMode::SpawnEnemyWave(bool bAggressiveGroup)
{
	int32 RemainingEnemiesToBeKilled = GoalToComplete->NpcSpawnRules.TotalNPCsToBeSpawned - EnemiesKilledCount;
	int32 EnemyCountThisWave;

	EnemyCountThisWave = GoalToComplete->NpcSpawnRules.GetNPCsPerWaveRand();

	if (!GoalToComplete->NpcSpawnRules.HasInfiniteWaves())
	{
		EnemyCountThisWave = FMath::Clamp(EnemyCountThisWave, 0, RemainingEnemiesToBeKilled);
	}
	
	for (int32 i = 0; i < EnemyCountThisWave; i++)
	{
		if (NavSystem && PlayerPawn)
		{
			FVector SpawnAreaOrigin = PlayerPawn->GetActorLocation();
			float   Radius          = GoalToComplete->NpcSpawnRules.NPCsSpawnAreaRadius;
			float   MinDistToOrigin = GoalToComplete->NpcSpawnRules.NPCsSpawnMinDistance;
			FVector SpawnLocation;

			DetermineNPCRandomSpawnLocation(SpawnAreaOrigin, Radius, MinDistToOrigin, SpawnLocation);

			SpawnSpacecraftNPC(GoalToComplete->NpcSpawnRules.GetNpcClassRand(), bAggressiveGroup, SpawnLocation);
		}
	}
}

void ACampaignGameMode::DetermineNPCRandomSpawnLocation(const FVector & Origin, float Radius,
	float MinDistanceToOrigin, FVector & Location)
{
	FNavLocation RandomNavLocation;

	do
	{
		NavSystem->GetRandomReachablePointInRadius(Origin, Radius, RandomNavLocation);

	} while ((RandomNavLocation.Location - Origin).Size() < MinDistanceToOrigin);

	Location = RandomNavLocation.Location;
}

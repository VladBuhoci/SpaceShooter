// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Listeners/GoalListener.h"

#include "CoreMinimal.h"
#include "GameModes/SpaceGameMode.h"
#include "CampaignGameMode.generated.h"

// Forward declarations.
class UGoalDescription;
class UUserWidget;


/**
 * Mission-based game mode.
 * Each mission consists of one or more chapters and it can be started from any of these chapters that the user wishes.
 */
UCLASS()
class SPACESHOOTER_API ACampaignGameMode : public ASpaceGameMode, public IGoalListener
{
	GENERATED_BODY()

	class USpaceGameInstance* SpaceGameInstance;
	class ACampaignHUD* CampaignHUD;

	/** Time (in seconds) until the end level stats widget appears. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space Game Mode | Campaign Game Mode", Meta = (AllowPrivateAccess = "true"))
	float LevelEndStatsHUDToggleWait;

	/** Goal to fulfill in order to move on to the next chapter. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode | Campaign Game Mode", Meta = (AllowPrivateAccess = "true"))
	UGoalDescription* GoalToComplete;

	/** Current amount of enemies destroyed in the current level. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode | Campaign Game Mode", Meta = (AllowPrivateAccess = "true"))
	int32 EnemiesKilledCount;

	/** Total number of enemies to be spawned in the level (excludes already-placed enemies). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode | Campaign Game Mode", Meta = (AllowPrivateAccess = "true"))
	int32 EnemiesToSpawnTotal;

public:
	/** Sets default values. */
	ACampaignGameMode();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	virtual void OnEnemySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft) override;
	virtual void OnEnemySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft) override;

	/** Called when all enemies in the current level have been wiped out. */
	virtual void OnSectorCleared() override;
	/** Called when the player's spacecraft has been destroyed. */
	virtual void OnPlayerDestroyed() override;

	/** Called when a chapter of the playing mission (excluding the final chapter) has been successfully completed. */
	void OnChapterCompleted();
	/** Called when the final chapter of the playing mission has been successfully completed. */
	void OnCampaignCompleted();


	/**********************************
		  GOAL LISTENER INTERFACE
	**********************************/

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Space Game Mode | Campaign Game Mode | Goal Listener Interface")
	void NotifyGoalCompleted();
	virtual void NotifyGoalCompleted_Implementation() override;

private:
	void OnChapterObjectiveDone();


	/**********************************
				 GETTERS
	**********************************/

public:
	int32 GetEnemiesKilledCount() const { return EnemiesKilledCount; }
	int32 GetEnemiesToSpawnTotal() const { return EnemiesToSpawnTotal; }
};

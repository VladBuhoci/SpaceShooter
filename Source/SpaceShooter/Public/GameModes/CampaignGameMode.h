// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameModes/SpaceGameMode.h"
#include "CampaignGameMode.generated.h"


/**
 * Mission-based game mode.
 * Each mission consists of one or more chapters and it can be started from any of these chapters that the user wishes.
 */
UCLASS()
class SPACESHOOTER_API ACampaignGameMode : public ASpaceGameMode
{
	GENERATED_BODY()
	
private:
	float GameOverHUDToggleWait;

public:
	/** Sets default values. */
	ACampaignGameMode();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called when all enemies in the current level have been wiped out. */
	virtual void OnSectorCleared() override;
	/** Called when the player's spacecraft has been destroyed. */
	virtual void OnPlayerDestroyed() override;

	/** Called when the final chapter of the playing mission has been successfully completed. */
	void OnCampaignCompleted();
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "GameModes/Gameplay/Missions.h"

#include "CoreMinimal.h"
#include "TextProperty.h"
#include "Engine/GameInstance.h"
#include "SpaceGameInstance.generated.h"


/**
 * A SpaceGameInstance object lives from the start of the game and until it is shut down.
 * It is a good place to store persistent data.
 */
UCLASS()
class SPACESHOOTER_API USpaceGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	TArray<UCampaignMissionDescription*> CampaignMissions;

	/** Current mission being played. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UMissionDescription* CurrentMission;

	/** Current chapter being played. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UChapterDescription* CurrentChapter;

	/** Next chapter to be played, if any exists. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Instance", Meta = (AllowPrivateAccess = "true"))
	UChapterDescription* NextChapter;

protected:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void AddCampaignMission(TSubclassOf<UCampaignMissionDescription> CampaignMissionClass, FText Name,
		UCampaignMissionDescription* & CreatedMission);
	
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void AddCampaignChapter(TSubclassOf<UCampaignChapterDescription> CampaignChapterClass, FText ChapterName, FName LevelName,
		UGoalDescription* Goal, UCampaignMissionDescription* CampaignMissionToAddTo, UCampaignMissionDescription* & CampaignMission);

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void CreateGoalOfTypeDestroyEveryone(TSubclassOf<UUserWidget> GoalWidgetType, int32 AmountOfEnemiesToKill,
		UDestroyEveryoneGoalDescription* & GenocideGoal);


	/**********************************
				 GETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void GetCampaignMissionsAvailable(TArray<UCampaignMissionDescription*> & Missions) { Missions = CampaignMissions; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UMissionDescription* GetCurrentMission() const { return CurrentMission; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UChapterDescription* GetCurrentChapter() const { return CurrentChapter; }

	UFUNCTION(BlueprintPure, Category = "Space Game Instance")
	UChapterDescription* GetNextChapterForCurrentMission();


	/**********************************
				 SETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void SetCurrentMission(UMissionDescription* NewMission);

	UFUNCTION(BlueprintCallable, Category = "Space Game Instance")
	void SetCurrentChapter(UChapterDescription* NewChapter);
};

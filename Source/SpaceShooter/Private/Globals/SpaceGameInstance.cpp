// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceGameInstance.h"


void USpaceGameInstance::AddCampaignMission(TSubclassOf<UCampaignMissionDescription> CampaignMissionClass,
	FText Name, UCampaignMissionDescription* & CreatedMission)
{
	if (!CampaignMissionClass)
		return;

	CreatedMission = NewObject<UCampaignMissionDescription>(this, FName(*("CampaignMission_" + Name.ToString())));

	if (CreatedMission)
	{
		CreatedMission->Name             = Name;
		CreatedMission->CampaignChapters = TArray<UCampaignChapterDescription*>();

		// After creation, add the mission to the game instance's array of missions.
		CampaignMissions.Add(CreatedMission);
	}
}

void USpaceGameInstance::AddCampaignChapter(TSubclassOf<UCampaignChapterDescription> CampaignChapterClass, FText ChapterName,
	FName LevelName, UGoalDescription* Goal, UCampaignMissionDescription* CampaignMissionToAddTo, UCampaignMissionDescription* & CampaignMission)
{
	if (!CampaignChapterClass || !CampaignMissionToAddTo)
		return;

	FString CampaignChapterObjUniqueName         = "CampaignChapter_" + ChapterName.ToString() + "_" + LevelName.ToString();
	UCampaignChapterDescription* CampaignChapter = NewObject<UCampaignChapterDescription>(this, FName(*CampaignChapterObjUniqueName));

	if (CampaignChapter)
	{
		CampaignChapter->ChapterName = ChapterName;
		CampaignChapter->LevelName   = LevelName;
		CampaignChapter->Goal        = Goal;

		CampaignMissionToAddTo->AddChapter(CampaignChapter);
	}

	// Useful in Blueprints for chained calls of this method.
	CampaignMission = CampaignMissionToAddTo;
}

void USpaceGameInstance::CreateGoalOfTypeDestroyEveryone(const FNpcSpawnRules & NpcSpawnRules,
	TSubclassOf<UUserWidget> GoalWidgetType, UDestroyEveryoneGoalDescription* & GenocideGoal)
{
	GenocideGoal = NewObject<UDestroyEveryoneGoalDescription>(this, FName(*("GenocideGoal_" + FString::FromInt(FMath::Rand()))));

	if (GenocideGoal)
	{
		GenocideGoal->SetWidgetClass(GoalWidgetType);
		GenocideGoal->NpcSpawnRules = NpcSpawnRules;
	}
}

UChapterDescription* USpaceGameInstance::GetNextChapterForCurrentMission()
{
	// First check if we cached a pointer to the next chapter.
	if (NextChapter)
		return NextChapter;

	// Otherwise, search for it and store it in case it exists.

	if (!CurrentMission || !CurrentChapter)
		return nullptr;

	TArray<UChapterDescription*> MissionChapters;
	int32 NextChapterIndex = -1;

	CurrentMission->GetCampaignChaptersAvailable(MissionChapters);

	for (UChapterDescription* Chapter : MissionChapters)
	{
		NextChapterIndex++;

		if (Chapter == CurrentChapter)
		{
			// Make sure the CurrentChapter is not the last one.
			// If it is though, return nullptr, signaling that we are at the end of the mission.

			if (NextChapterIndex < MissionChapters.Num() - 1)
				return NextChapter = MissionChapters[NextChapterIndex + 1];
			else
				return nullptr;
		}
	}

	return nullptr;
}

void USpaceGameInstance::SetCurrentMission(UMissionDescription* NewMission)
{
	CurrentMission = NewMission;
	CurrentChapter = nullptr;
	NextChapter    = nullptr;
}

void USpaceGameInstance::SetCurrentChapter(UChapterDescription* NewChapter)
{
	CurrentChapter = NewChapter;
	NextChapter    = nullptr;
}

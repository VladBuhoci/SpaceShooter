// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "GameModes/Gameplay/Chapters.h"

#include "CoreMinimal.h"
#include "Missions.generated.h"


UCLASS(BlueprintType)
class SPACESHOOTER_API UMissionDescription : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission Description", Meta = (AllowPrivateAccess = "true"))
	FText Name;


	/**********************************
				 GETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Mission Description")
	virtual void GetCampaignChaptersAvailable(TArray<UChapterDescription*> & Chapters) {}

	virtual void AddChapter(UChapterDescription* NewChapter) {}
};

UCLASS(BlueprintType)
class SPACESHOOTER_API UCampaignMissionDescription : public UMissionDescription
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission Description", Meta = (AllowPrivateAccess = "true"))
	TArray<UCampaignChapterDescription*> CampaignChapters;


	/**********************************
				 GETTERS
	**********************************/

public:
	UFUNCTION(BlueprintCallable, Category = "Mission Description")
	virtual void GetCampaignChaptersAvailable(TArray<UChapterDescription*> & Chapters) override
	{
		Chapters.Append(CampaignChapters);
	}

	virtual void AddChapter(UChapterDescription* NewChapter) override
	{
		if (UCampaignChapterDescription* NewCampaignChapter = Cast<UCampaignChapterDescription>(NewChapter))
			CampaignChapters.Add(NewCampaignChapter);
	}
};

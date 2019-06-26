// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "GameModes/Gameplay/DestroyEveryoneGoalDescription.h"

#include "CoreMinimal.h"
#include "Chapters.generated.h"


UCLASS(BlueprintType)
class SPACESHOOTER_API UChapterDescription : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chapter Description", Meta = (AllowPrivateAccess = "true"))
	FText ChapterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chapter Description", Meta = (AllowPrivateAccess = "true"))
	FName LevelName;

public:
	virtual UGameplaySpecification* GetGameplaySpecification() const { return nullptr; }
};

UCLASS(BlueprintType)
class SPACESHOOTER_API UCampaignChapterDescription : public UChapterDescription
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chapter Description")
	UGoalDescription* Goal;

public:
	virtual UGameplaySpecification* GetGameplaySpecification() const override { return Goal; }
};

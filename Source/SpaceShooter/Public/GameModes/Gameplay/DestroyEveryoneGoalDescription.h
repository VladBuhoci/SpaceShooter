// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "GameModes/Gameplay/GoalDescription.h"
#include "DestroyEveryoneGoalDescription.generated.h"


UCLASS(BlueprintType)
class SPACESHOOTER_API UDestroyEveryoneGoalDescription : public UGoalDescription
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Goal Description")
	int32 TotalEnemiesToKill;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Goal Description")
	int32 CurrentKilledEnemiesCount;

public:
	virtual void UpdateGoal(EGoalUpdateType UpdateType) override;

	virtual void ResetGoalProgress() override;
};

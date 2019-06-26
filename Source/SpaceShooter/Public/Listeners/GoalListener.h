// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "GoalListener.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGoalListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing classes will update the goal's progress and the goal will notify them back when it is completed.
 */
class SPACESHOOTER_API IGoalListener
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called when the progress of this goal has been completed. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Goal Listener")
	void NotifyGoalCompleted();
};

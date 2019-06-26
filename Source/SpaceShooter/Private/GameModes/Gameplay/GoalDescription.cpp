// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/Gameplay/GoalDescription.h"


void UGoalDescription::RegisterListener(IGoalListener* NewListener)
{
	if (NewListener && !GoalListeners.Contains(NewListener))
		GoalListeners.Add(NewListener);
}

void UGoalDescription::UnregisterListener(IGoalListener* ListenerToRemove)
{
	if (ListenerToRemove && GoalListeners.Contains(ListenerToRemove))
		GoalListeners.Remove(ListenerToRemove);
}

void UGoalDescription::NotifyGoalListeners()
{
	while (GoalListeners.Num() > 0)
	{
		IGoalListener* Listener = GoalListeners.Pop();
		
		if (UObject* ListenerAsObj = Cast<UObject>(Listener))
			if (ListenerAsObj->IsValidLowLevel())
				IGoalListener::Execute_NotifyGoalCompleted(ListenerAsObj);
	}
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/Gameplay/DestroyEveryoneGoalDescription.h"


void UDestroyEveryoneGoalDescription::UpdateGoal(EGoalUpdateType UpdateType)
{
	if (UpdateType == EGoalUpdateType::EnemyDestroyed)
	{
		CurrentKilledEnemiesCount = FMath::Clamp(CurrentKilledEnemiesCount + 1, 0, NpcSpawnRules.TotalNPCsToBeSpawned);

		if (CurrentKilledEnemiesCount == NpcSpawnRules.TotalNPCsToBeSpawned)
			NotifyGoalListeners();
	}
}

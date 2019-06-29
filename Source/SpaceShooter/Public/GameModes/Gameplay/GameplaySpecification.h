// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceStructs.h"

#include "CoreMinimal.h"
#include "GameplaySpecification.generated.h"


UCLASS(BlueprintType)
class SPACESHOOTER_API UGameplaySpecification : public UObject
{
	GENERATED_BODY()

public:
	/** Describes NPC spawning for a game mode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Specification")
	FNpcSpawnRules NpcSpawnRules;
};

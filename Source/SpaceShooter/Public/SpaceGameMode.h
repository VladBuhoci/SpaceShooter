// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameMode.generated.h"

/**
 * The class which defines the Space game rules and core actors that will spawn on map load.
 */
UCLASS()
class SPACESHOOTER_API ASpaceGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	/** Sets default values. */
	ASpaceGameMode();
};

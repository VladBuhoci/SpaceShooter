// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

/**
 * The "game mode" of the main menu.
 */
UCLASS()
class SPACESHOOTER_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	/** Sets default values. */
	AMainMenuGameMode();
};

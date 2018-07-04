// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "MainMenuGameMode.h"
#include "MainMenuPlayerPawn.h"


/** Sets default values. */
AMainMenuGameMode::AMainMenuGameMode()
{
	DefaultPawnClass = AMainMenuPlayerPawn::StaticClass();
}

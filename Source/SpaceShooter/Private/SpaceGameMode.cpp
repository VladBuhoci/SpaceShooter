// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceGameMode.h"
#include "SpacePlayerPawn.h"
#include "SpacePlayerController.h"
#include "SpaceHUD.h"


/** Sets default values. */
ASpaceGameMode::ASpaceGameMode()
{
	DefaultPawnClass      = ASpacePlayerPawn::StaticClass();
	PlayerControllerClass = ASpacePlayerController::StaticClass();
	HUDClass              = ASpaceHUD::StaticClass();
}

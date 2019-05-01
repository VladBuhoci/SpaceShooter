// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuPlayerPawn.generated.h"

/**
 * The player class which is spawned when the main menu level is loaded.
 */
UCLASS()
class SPACESHOOTER_API AMainMenuPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	/** Sets default values. */
	AMainMenuPlayerPawn();
};

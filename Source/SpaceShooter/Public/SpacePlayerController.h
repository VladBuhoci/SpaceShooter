// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpacePlayerController.generated.h"

/**
 * Space Player Pawn controller.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/** Sets default values. */
	ASpacePlayerController();

	/** Called to bind functionality to input. */
	virtual void SetupInputComponent() override;
};

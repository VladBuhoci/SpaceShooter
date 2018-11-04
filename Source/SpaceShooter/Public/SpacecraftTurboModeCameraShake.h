// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraShake.h"
#include "SpacecraftTurboModeCameraShake.generated.h"


/**
 * Defines a set of parameters that will control the camera shake effect.
 */
UCLASS()
class SPACESHOOTER_API USpacecraftTurboModeCameraShake : public UCameraShake
{
	GENERATED_BODY()
	
public:
	USpacecraftTurboModeCameraShake();
};

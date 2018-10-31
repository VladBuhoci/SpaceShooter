// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Object.h"
#include "SpaceEnums.generated.h"


/**
 * The "side" to which a spacecraft belongs to.
 */
UENUM(BlueprintType)
enum class ESpacecraftFaction : uint8
{
	/** Not belonging to any side. */
	Unspecified,

	/** The human race. */
	Human,

	/** The alien race. */
	Alien
};


/**
 * 
 */
UCLASS()
class SPACESHOOTER_API USpaceEnums : public UObject
{
	GENERATED_BODY()

public:
	USpaceEnums(const FObjectInitializer & ObjectInitializer);
};

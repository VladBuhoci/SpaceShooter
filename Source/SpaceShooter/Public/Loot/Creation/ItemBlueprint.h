// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"
#include "Globals/ItemAttributes.h"

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemBlueprint.generated.h"


/**
 * Base class for item blueprints, used by the item builders to spawn particular loot (ammo piles, weapons etc.).
 */
UCLASS()
class SPACESHOOTER_API UItemBlueprint : public UObject
{
	GENERATED_BODY()
	
};

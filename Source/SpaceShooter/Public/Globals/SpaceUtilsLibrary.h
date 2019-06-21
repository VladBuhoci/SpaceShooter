// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SpaceUtilsLibrary.generated.h"


/**
 * Contains various utility static methods.
 */
UCLASS()
class SPACESHOOTER_API USpaceUtilsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FText GetFloatAsText(float Number, int32 Precision = 2, bool IncludeLeadingZero = true);
};

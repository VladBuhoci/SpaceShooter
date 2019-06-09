// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WorldAwareObject.generated.h"


UCLASS()
class SPACESHOOTER_API UWorldAwareObject : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override { return GetOuter() != nullptr ? GetOuter()->GetWorld() : nullptr; }
};

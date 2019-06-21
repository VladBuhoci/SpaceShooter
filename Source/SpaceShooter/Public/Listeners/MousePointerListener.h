// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "MousePointerListener.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMousePointerListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Listens to mouse movement and reacts accordingly.
 */
class SPACESHOOTER_API IMousePointerListener
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called when the mouse cursor begins pointing at this entity. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mouse Pointer Listener")
	void OnMouseEnter();

	/** Called when the mouse cursor stops pointing at this entity. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Mouse Pointer Listener")
	void OnMouseLeave();
};

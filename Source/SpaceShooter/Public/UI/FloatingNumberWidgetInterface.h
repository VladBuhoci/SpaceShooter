// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "FloatingNumberWidgetInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFloatingNumberWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Widgets implementing this are expected to display a number onto the viewport in some way (widgets may also be animated, hence the position given via SetInitialPosition(const FVector &) is considered to be "initial").
 */
class SPACESHOOTER_API IFloatingNumberWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Sets the starting position of the widget in the viewport. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Floating Number Widget Interface")
	void SetInitialPosition(const FVector2D & InitialPosition);

	/** Sets the value of the number that will be shown by the widget. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Floating Number Widget Interface")
	void SetDisplayedNumberValue(int32 Value);

	/** Sets the colour of the number that will be shown by the widget. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Floating Number Widget Interface")
	void SetDisplayedNumberColour(const FColor & Colour);
};

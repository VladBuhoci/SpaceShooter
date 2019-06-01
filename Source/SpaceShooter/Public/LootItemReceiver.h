// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "LootItemReceiver.generated.h"

// Forward declarations.
class AItem;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULootItemReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing classes can receive items via the Supply(AItem*) method.
 */
class SPACESHOOTER_API ILootItemReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Offers an item of any kind to an entity and lets it handle the receiving in its own way. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Loot Item Receiver")
	void Supply(AItem* ItemToProvide);
};

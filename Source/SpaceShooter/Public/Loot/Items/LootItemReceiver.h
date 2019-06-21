// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/SpaceEnums.h"

#include "CoreMinimal.h"
#include "LootItemReceiver.generated.h"

// Forward declarations.
class AItem;


/**
 * Describes what happens when a spacecraft interacts with a loot chest attempting to grab the highlighted item inside.
 */
UENUM(BlueprintType)
enum class EItemTakingAction : uint8
{
	None,

	/** For things such as ammo piles/money (or other groups), which can be taken entirely or just a few units at a time. */
	PartiallyTaken,

	/** When the item has been taken and it should be erased from where it was found (loot chests). */
	FullyTaken
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULootItemReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implementing classes can receive items via the Supply(AItem*, EItemTakingAction&) method.
 */
class SPACESHOOTER_API ILootItemReceiver
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Offers an item of any kind to an entity and lets it handle the receiving in its own way. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Loot Item Receiver")
	void Supply(AItem* ItemToProvide, EItemTakingAction & ItemTakeAction);
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Inventory.generated.h"

// Forward declarations.
class AItem;


USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	AItem* StoredItem;

	bool operator >(const FInventorySlot& OtherSlot) const;
};

UCLASS()
class SPACESHOOTER_API UInventory : public UObject
{
	GENERATED_BODY()

private:
	TArray<FInventorySlot> ItemSlots;

	int32 FirstEmptySlotIndex;
	int32 CurrentlyHighlightedSlotIndex;

public:
	/** Sets default values. */
	UInventory();

public:
	void SetUpInventorySlots(int32 SlotsCount);
	void ResetHighlightedSlotIndex();
	
	bool AddItem(AItem* ItemToAdd, bool bShouldTriggerItemSorting);
	void AddItems(TArray<AItem*> ItemsToAdd);
	
	void GetAllItems(TArray<AItem*> & Items);
	AItem* GetItemAt(int32 Index);

	/**
	 * Puts the given item at the specified index (if they're both valid).
	 * 
	 * @returns The previous item sitting in that slot, if it wasn't empty.
	 */
	template<typename T>
	T* ReplaceItemAtIndex(int32 SlotIndex, T* ItemToPlaceInSlot, bool bShouldTriggerItemSorting);

	/**
	 * Removes the item at the given index (if it's valid).
	 *
	 * @returns The item sitting in that slot, if it wasn't empty.
	 */
	AItem* RemoveItem(int32 SlotIndex);

	bool IsFull() const { return FirstEmptySlotIndex == ItemSlots.Num(); }

private:
	void SortInventory();


	/**********************************
			     GETTERS
	**********************************/

public:
	int32 GetCurrentlyHighlightedSlotIndex() const { return CurrentlyHighlightedSlotIndex; }
};

template<typename T>
inline T* UInventory::ReplaceItemAtIndex(int32 SlotIndex, T* ItemToPlaceInSlot, bool bShouldTriggerItemSorting)
{
	if (SlotIndex < 0 || SlotIndex > ItemSlots.Num() - 1 || (ItemToPlaceInSlot && !ItemToPlaceInSlot->GetClass()->IsChildOf<AItem>()))
		return nullptr;
	
	T* PreviousItem = Cast<T>(ItemSlots[SlotIndex].StoredItem);

	ItemSlots[SlotIndex].StoredItem = ItemToPlaceInSlot;

	if (bShouldTriggerItemSorting)
	{
		SortInventory();
	}

	return PreviousItem;
}

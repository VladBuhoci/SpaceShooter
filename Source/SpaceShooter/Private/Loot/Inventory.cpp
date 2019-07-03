// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Inventory.h"
#include "Loot/Items/Item.h"


// Struct implementation.

bool FInventorySlot::operator>(const FInventorySlot & OtherSlot) const
{
	return this->StoredItem == nullptr ? false
		: OtherSlot.StoredItem == nullptr ? true
		: this->StoredItem->GetItemRarity()->GetRarityGrade() > OtherSlot.StoredItem->GetItemRarity()->GetRarityGrade();
}

// Class implementation.

UInventory::UInventory()
{
	FirstEmptySlotIndex = 0;
}

void UInventory::SetUpInventorySlots(int32 SlotsCount)
{
	for (int32 i = 0; i < SlotsCount; i++)
	{
		ItemSlots.Add(FInventorySlot());
	}
}

void UInventory::ResetHighlightedSlotIndex()
{
	CurrentlyHighlightedSlotIndex = 0;
}

bool UInventory::AddItem(AItem* ItemToAdd, bool bShouldTriggerItemSorting)
{
	UE_LOG(LogTemp, Log, TEXT("Trying to add a new item to the inventory (First empty slot: %d | Array size: %d)"),
		FirstEmptySlotIndex, ItemSlots.Num())

	if (IsFull())
		return false;

	if (ItemToAdd)
	{
		ItemSlots[FirstEmptySlotIndex].StoredItem = ItemToAdd;

		FirstEmptySlotIndex++;

		if (bShouldTriggerItemSorting)
		{
			SortInventory();
		}

		return true;
	}

	return false;
}

void UInventory::AddItems(TArray<AItem*> ItemsToAdd)
{
	if (IsFull())
		return;

	if (ItemsToAdd.Num() > 0)
	{
		for (AItem* Item : ItemsToAdd)
			AddItem(Item, false);
		
		SortInventory();
	}
}

void UInventory::GetAllItems(TArray<AItem*> & Items)
{
	for (int32 i = 0; i < ItemSlots.Num(); i++)
	{
		Items.Add(ItemSlots[i].StoredItem);
	}
}

AItem* UInventory::GetItemAt(int32 Index)
{
	if (Index < 0 || Index > ItemSlots.Num() - 1)
		return nullptr;

	return ItemSlots[Index].StoredItem;
}

AItem* UInventory::RemoveItem(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex > ItemSlots.Num() - 1)
		return nullptr;

	AItem* DiscardedItem = nullptr;

	if (ItemSlots[SlotIndex].StoredItem)
	{
		DiscardedItem = ItemSlots[SlotIndex].StoredItem;
		
		ItemSlots.RemoveAt(SlotIndex);
		ItemSlots.Add(FInventorySlot());

		FirstEmptySlotIndex = FMath::Clamp(FirstEmptySlotIndex - 1, 0, ItemSlots.Num() - 1);
		CurrentlyHighlightedSlotIndex = FirstEmptySlotIndex;
	}

	return DiscardedItem;
}

void UInventory::SortInventory()
{
	if (ItemSlots.Num() > 0)
	{
		ItemSlots.Sort([](const FInventorySlot& LHS, const FInventorySlot& RHS) { return LHS > RHS; });
	}
}

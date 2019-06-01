// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MousePointerListener.h"
#include "LootChest.generated.h"

// Forward declarations.
class ILootItemReceiver;
class UWidgetComponent;

class UXYOnlyPhysicsConstraintComponent;
class AItem;
class UItemPoolListDefinition;


UENUM(BlueprintType)
enum class ELootChestState : uint8
{
	/** The state of the chest before the player interacts with it for the first time. */
	Closed,

	/** The state in-between Closed and Opened. The chest cannot be interacted with while in this state. */
	Opening,

	/** The state of the chest after the player interacts with it for the first time, opening it. */
	Opened
};

/**
 * Base class for loot chests.
 */
UCLASS()
class SPACESHOOTER_API ALootChest : public AActor, public IMousePointerListener
{
	GENERATED_BODY()

	/** Mesh of this chest. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChestMeshComponent;

	/** Physical constraint component for this loot chest. Primarily used to keep it in the XY plane. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UXYOnlyPhysicsConstraintComponent* XYPlanePhysicsConstraintComponent;

	/** Widget component presenting the name of this chest in a simple widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InfoWidgetComponent;

	/** Name of this loot chest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	FText ChestName;

	/** Current state of this chest. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	ELootChestState CurrentState;
	
	/** Animation asset played when this chest is being opened. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UAnimSequence* OpenAnimation;

	/** Time to pass (in seconds) before physics simulations are turned off for this chest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float TimeBeforePhysicsOff;

	/** Widget component presenting the items of this chest in a simple overview-like widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ItemsOverviewWidgetComponent;

	/** Widget component presenting the items of this chest in a simple overview-like widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> ContainedItems;

	/** Index of the highlighted item at the moment. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	int32 CurrentlySelectedItemIndex;

public:
	/** Sets default values. */
	ALootChest();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

private:
	void BeginPhysicsSimulation();
	void EndPhysicsSimulation();


	/**********************************
		 MOUSE LISTENER INTERFACE
	**********************************/

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Loot Chest | Mouse Listener Interface")
	void OnMouseEnter();
	virtual void OnMouseEnter_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Loot Chest | Mouse Listener Interface")
	void OnMouseLeave();
	virtual void OnMouseLeave_Implementation() override;

protected:
	/** True if the mouse cursor is located over this object at the moment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest | Mouse Listener Interface")
	bool bCurrentlyPointedAt;

	/** True if the chest's contained items are currently being seen by someone (implies this chest is opened). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest | Mouse Listener Interface")
	bool bCurrentlyBeingInspected;


	/**********************************
		   LOOT CHEST INTERFACE
	**********************************/

protected:
	/** Item pool list definition for this chest type. The chest will be filled with items from this list. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest")
	TSubclassOf<UItemPoolListDefinition> LootDefinitionClass;

public:
	void Interact(ILootItemReceiver* ReceivingPawn);
	void HighlightPreviousItemInsideChest();
	void HighlightNextItemInsideChest();

protected:
	/** Called just before the chest is getting opened. Good for setting things up such as the Items Overview widget. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnChestPreOpen();
	
	/** Called when receiving a request to select (highlight/move to) the previous item in the list. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnHighlightPreviousItemInsideChest();
	
	/** Called when receiving a request to select (highlight/move to) the next item in the list. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnHighlightNextItemInsideChest();

	/**
	 * Called when receiving a request to grab the currently highlighted item in the list.
	 * 
	 * @param RemovedHighlightedItemIndex index of the previously highlighted item which has been grabbed now
			and removed from the array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnGrabHighlightedItemFromChest(int32 RemovedHighlightedItemIndex, bool bRemoveItem);

private:
	void PresentChestIdentity();
	void HideChestIdentity();
	void BeginChestInspection();
	void EndChestInspection();

	void GenerateItems();
	void OpenChest();
	void GrabHighlightedItemFromChest(ILootItemReceiver* ReceivingPawn);

	bool AreItemsLeft() const;


	/**********************************
				GETTERS
	**********************************/

public:
	/** Returns the stored items from this chest. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	TArray<AItem*> GetContainedItems() const { return ContainedItems; }

	/** Returns the index of the highlighted item from this chest during inspections. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	int32 GetCurrentItemIndex() const { return CurrentlySelectedItemIndex; }
};

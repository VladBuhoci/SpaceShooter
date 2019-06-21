// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Listeners/MousePointerListener.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootChest.generated.h"

// Forward declarations.
class UWidgetComponent;
class UAnimSequence;
class UPointLightComponent;

class ILootItemReceiver;
class UXYOnlyPhysicsConstraintComponent;
class AItem;
class AItemBox;
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

	/** Point light source. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UPointLightComponent* PointLightComponent;

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

	/** Intensity of the light for when the chest is empty. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensityLow;

	/** Intensity of the light for most of the time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensityNormal;

	/** Intensity of the light for moments such as when the chest is spawned or when something is taken from it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensityHigh;

	/** Intensity value that the point light will soon have. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensityCurrentTargetValue;

	/** Higher values will make the transition from one light intensity level to the other take less time. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensitySwapSpeed;

	/** Time (in seconds) it takes to change the targeted light intensity value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float LightIntensitySwapInterval;

	/** Keeps track of the timer that triggers a change in the light intensity's value. */
	FTimerHandle LightIntensityTimerHandle;

	/** Time to pass (in seconds) before physics simulations are turned off for this chest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float TimeBeforePhysicsOff;

	/** Widget component presenting the item boxes (more like items) of this chest in a simple overview-like widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ChestOverviewWidgetComponent;

	/** Array of the item boxes inside this treasure chest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	TArray<AItemBox*> ContainedItemBoxes;

	/** Index of the highlighted item box at the moment. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	int32 CurrentlySelectedItemBoxIndex;

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
	void HandleLightIntensity(float DeltaTime);
	
	void SetLightIntensityLevel(float NewIntensityTargetValue);
	void SetLightIntensityLevel(float NewTemporaryIntensityTargetValue, float FuturePermanentIntensityTargetValue);

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

	/** True if the chest's contained item boxes are currently being seen by someone (implies this chest is opened). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest | Mouse Listener Interface")
	bool bCurrentlyBeingInspected;


	/**********************************
		   LOOT CHEST INTERFACE
	**********************************/

protected:
	/** Item pool list definition for this chest type. The chest will be filled with boxes containing items from this list. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest")
	TSubclassOf<UItemPoolListDefinition> LootDefinitionClass;

public:
	void Interact(ILootItemReceiver* ReceivingPawn);
	void HighlightPreviousItemBoxInsideChest();
	void HighlightNextItemBoxInsideChest();

protected:
	/** Called just before the chest is getting opened. Good for setting things up such as the Treasure Chest Overview widget. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnChestPreOpen();
	
	/** Called when receiving a request to select (highlight/move to) the previous item box in the list. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnHighlightPreviousItemBoxInsideChest();
	
	/** Called when receiving a request to select (highlight/move to) the next item box in the list. */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnHighlightNextItemBoxInsideChest();

	/**
	 * Called when receiving a request to grab the currently highlighted item box in the list.
	 * 
	 * @param RemovedHighlightedItemBoxIndex index of the previously highlighted item box whose
			item has been grabbed now and is removed from the array.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Loot Chest")
	void OnGrabHighlightedItemBoxFromChest(int32 RemovedHighlightedItemBoxIndex, bool bRemoveItem);

private:
	void PresentChestIdentity();
	void HideChestIdentity();
	void BeginChestInspection();
	void EndChestInspection();

	void GenerateItemsAndItemBoxes();
	void OpenChest();
	void GrabHighlightedItemBoxFromChest(ILootItemReceiver* ReceivingPawn);

	bool AreItemsLeft() const;


	/**********************************
				GETTERS
	**********************************/

public:
	/** Returns the stored item boxes from this chest. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	TArray<AItemBox*> GetContainedItemBoxes() const { return ContainedItemBoxes; }

	/** Returns the index of the highlighted item box from this chest during inspections. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	int32 GetCurrentItemBoxIndex() const { return CurrentlySelectedItemBoxIndex; }
};

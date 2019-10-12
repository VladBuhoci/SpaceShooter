// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Pawns/SpacecraftPawn.h"
#include "Loot/Items/LootItemReceiver.h"

#include "CoreMinimal.h"
#include "SpacePlayerPawn.generated.h"

// Forward declarations.
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class UParticleSystemComponent;

class ALootChest;
class AItemBox;
class UInventory;


/**
 * Represents the player in the space levels.
 * It is instantiated whenever a playable level is loaded.
 */
UCLASS()
class SPACESHOOTER_API ASpacePlayerPawn : public ASpacecraftPawn, public ILootItemReceiver
{
	GENERATED_BODY()

private:
	/**
	 * A component that is not rendered in game.
	 * This object attaches to the spaceship while maintaining a zero-rotation relative to the world.
	 * For the SpacePlayerPawn subclass, this is very useful because the camera's spring arm focuses on this component,
	 *		therefore the camera will not rotate when the spaceship does so.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USceneComponent* CentralSceneComponent;

	/** The camera that is used to view the player and the world it belongs to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	/**
	 * The spring arm is used to keep the camera away at a certain distance from the player,
	 *		retracting or extending when (opaque) obstacles of any sort obstruct the camera's view.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;

	/*
	 * Used to detect whenever a loot chest gets close enough to the spacecraft so it can be auto-interacted with.
	 * For now, only ammo piles will be auto-picked up from the chest.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USphereComponent* LootChestQuickInteractArea;

	/** Determines if the spacecraft can interact with any loot chests at all. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	bool bCanInteract;

	/** Types of item boxes that will be grabbed from nearby loot chests (inside quick-interactions area). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AItemBox>> TypesToPickUp;

public:
	/** Sets default values. */
	ASpacePlayerPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


	/**********************************
			   LOOT INTERFACE
	**********************************/

public:
	/** Searches for and picks up items from nearby loot chests. */
	void SearchAndCollectNearbySupplies();

protected:
	/** Called when a loot chest enters this player spacecraft's detection (auto-pickup) area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spacecraft | Loot")
	void OnLootChestEnterAutoInteractArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void OnLootChestEnterAutoInteractArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	void NearbyLootChestsCheckUp();
	void LookForNearbyLootChestsForAutomaticPickUp(TArray<ALootChest*> & NearbyChests);
	void PickUpItemsFromLootChest(ALootChest* ChestToLoot);


	/**********************************
		     MOVEMENT INTERFACE
	**********************************/

protected:
	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

protected:
	/**
	 * Called after taking damage, a value not great enough to be destroyed yet.
	 * Damage has already been applied and checks done at the time of this method being called.
	 */
	virtual void OnDamageTaken(ASpacecraftPawn* DamageCauser, int32 DamageTaken) override;

	/**
	 * Called right before the spacecraft is removed from the world.
	 * 
	 * @param bShouldPlayDestroyEffects [ref] if set to true, an explosion will be spawned at the spacecraft's location.
	 * @param bShouldBeDestroyedForGood [ref] if set to false, the attempt to remove this actor is canceled.
	 */
	virtual void PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed) override;


	/**********************************
		     WEAPONS INTERFACE
	**********************************/

public:
	/** Activates equipped weapon on the spacecraft. */
	virtual void BeginFiringWeapon() override;

	/** Deactivates equipped weapon on the spacecraft. */
	virtual void EndFiringWeapon() override;


	/**********************************
		    INVENTORY INTERFACE
	**********************************/

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Inventory", Meta = (AllowPrivateAccess = "true"))
	UInventory* Inventory;

protected:
	int32 InventorySlotsCount;

public:
	UFUNCTION(BlueprintCallable, Category = "Spacecraft | Inventory")
	void GetAllItemsFromInventory(TArray<AItem*> & Items);

	UFUNCTION(BlueprintCallable, Category = "Spacecraft | Inventory")
	AItem* GetItemFromInventoryAt(int32 Index);

	/**
	 * Swaps the two weapons.
	 * NOTE: the ActiveSlotIndex 1 & 2 values should be between 1 and 4, not 0 and 3!
	 */
	UFUNCTION(BlueprintCallable, Category = "Spacecraft | Inventory")
	void SwapActiveWeaponWithActiveWeapon(int32 ActiveSlotIndex1, int32 ActiveSlotIndex2);

	/**
	 * Swaps the two weapons.
	 * NOTE: the ActiveSlotIndex value should be between 1 and 4, not 0 and 3!
	 */
	UFUNCTION(BlueprintCallable, Category = "Spacecraft | Inventory")
	void SwapActiveWeaponWithInventoryWeapon(int32 ActiveSlotIndex, int32 InventorySlotIndex);

	/** Removes the item at the indicated slot, if the provided index is valid (index is zero-based). */
	UFUNCTION(BlueprintCallable, Category = "Spacecraft | Weapons")
	void RemoveItemFromInventory(int32 SlotIndex);

private:
	bool IsSpaceAvailableOnSpacecraft();
	bool IsSpaceAvailableInInventory();

protected:
	virtual void AddItemToInventory(AItem* NewItem) override;
	virtual void OnActiveWeaponSlotsStateChanged() override;
	
	void OnInventoryStateChanged();

	/** Destructs the weapons attached to the spacecraft. */
	virtual void DestroyWeaponry() override;

	/**
	 * Loot Item Receiver Interface implementation.
	 */
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spacecraft | Loot Item Receiver Interface")
	void Supply(AItem* ItemToProvide, EItemTakingAction & ItemTakeAction);
	virtual void Supply_Implementation(AItem* ItemToProvide, EItemTakingAction & ItemTakeAction) override;

	bool CanInteract() const { return bCanInteract; }
	void SetCanInteract(bool bNewState) { bCanInteract = bNewState; }


	/**********************************
			 CAMERA INTERFACE
	**********************************/

private:
	/** The length value for the camera's spring arm is calculated as SpeedPercentage * Spacecraft Velocity. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float SpringArmLength_SpeedFactor;

	/**
	 * The length value for the camera's spring arm is calculated as SpringArmLength_SpeedFactor * Spacecraft Velocity.
	 * Then we multiply it using this property to get the final result (during normal flight/idle mode only).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float SpringArmLength_NormalFlightModeMultiplier;

	/**
	 * The length value for the camera's spring arm is calculated as SpringArmLength_SpeedFactor * Spacecraft Velocity.
	 * Then we multiply it using this property to get the final result (during turbo flight mode only).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float SpringArmLength_TurboFlightModeMultiplier;

	/** Camera's field of view during idle or normal (slow) flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraNormalFlightFOV;

	/** Camera's field of view during turbo (fast) flight mode. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraTurboFlightFOV;

	/** The speed at which the spring arm of the camera changes its length during flight. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraZoomSpeed;

	/** The speed at which the FOV of the camera changes its angle during flight. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | Player Camera", Meta = (AllowPrivateAccess = "true"))
	float CameraFOVZoomSpeed;

	void CheckCameraOffset(float DeltaTime);
};

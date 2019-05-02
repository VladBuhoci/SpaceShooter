// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MousePointerListener.h"
#include "LootChest.generated.h"

// Forward declarations.
class UWidgetComponent;

class UXYOnlyPhysicsConstraintComponent;


UENUM(BlueprintType)
enum class ELootChestState : uint8
{
	/** The state of the chest before the player interacts with it for the first time. */
	Closed,

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	FText ChestName;

	/** Current state of this chest. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	ELootChestState CurrentState;
	
	/** Time to pass (in seconds) before physics simulations are turned off for this chest. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	float TimeBeforePhysicsOff;

public:	
	/** Sets default values. */
	ALootChest();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:	
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;


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
};

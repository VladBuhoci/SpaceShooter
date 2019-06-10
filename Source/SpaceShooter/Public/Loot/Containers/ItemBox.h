// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBox.generated.h"

// Forward declarations.
class AItem;


/**
 * Base class for item boxes wrapping up item actors.
 */
UCLASS()
class SPACESHOOTER_API AItemBox : public AActor
{
	GENERATED_BODY()
	
private:
	/** Mesh of this box. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* BoxMeshComponent;

public:	
	/** Sets default values. */
	AItemBox();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:	
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

	
	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns the item wrapped by this box. */
	UFUNCTION(BlueprintPure, Category = "Item Box")
	virtual AItem* GetContainedItem() const { return nullptr; }

	/**********************************
				 SETTERS
	**********************************/

public:
	/** Sets the item wrapped by this box. */
	UFUNCTION(BlueprintCallable, Category = "Item Box")
	virtual void SetContainedItem(AItem* NewItem) {}
};

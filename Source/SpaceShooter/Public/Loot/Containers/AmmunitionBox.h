// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Containers/ItemBox.h"

#include "CoreMinimal.h"
#include "AmmunitionBox.generated.h"

// Forward declarations.
class AAmmunitionPile;


/**
 * Item box holding an AAmmunitionPile object.
 */
UCLASS()
class SPACESHOOTER_API AAmmunitionBox : public AItemBox
{
	GENERATED_BODY()
	
private:
	/** Item inside this item box. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Box | Ammunition Box", Meta = (AllowPrivateAccess = "true"))
	AAmmunitionPile* ContainedItem;


	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns the item wrapped by this box. */
	UFUNCTION(BlueprintPure, Category = "Item Box | Ammunition Box")
	virtual AItem* GetContainedItem() const override;

	/**********************************
				 SETTERS
	**********************************/

public:
	/** Sets the item wrapped by this box. */
	UFUNCTION(BlueprintCallable, Category = "Item Box | Ammunition Box")
	virtual void SetContainedItem(AItem* NewItem) override;
};

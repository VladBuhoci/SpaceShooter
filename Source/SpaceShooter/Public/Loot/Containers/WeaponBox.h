// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Containers/ItemBox.h"

#include "CoreMinimal.h"
#include "WeaponBox.generated.h"

// Forward declarations.
class AWeapon;


/**
 * Item box holding an AWeapon object.
 */
UCLASS()
class SPACESHOOTER_API AWeaponBox : public AItemBox
{
	GENERATED_BODY()
	
	private:
	/** Item inside this item box. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Box | Weapon Box", Meta = (AllowPrivateAccess = "true"))
	AWeapon* ContainedItem;


	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns the item wrapped by this box. */
//	UFUNCTION(BlueprintPure, Category = "Item Box | Weapon Box")
	virtual AItem* GetContainedItem() const override;

	/**********************************
				 SETTERS
	**********************************/

public:
	/** Sets the item wrapped by this box. */
//	UFUNCTION(BlueprintCallable, Category = "Item Box | Weapon Box")
	virtual void SetContainedItem(AItem* NewItem) override;
};

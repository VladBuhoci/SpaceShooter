// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Globals/ItemAttributes.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// Forward declarations.
class UItemRarity;


/**
 * Base class of all player-usable items (ammo, weapons etc).
 */
UCLASS()
class SPACESHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	/** Name of this item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	FText Name;

	/** Icon of this item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	UTexture2D* Icon;

	/** Rarity of this item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	UItemRarity* Rarity;

public:	
	/** Sets default values. */
	AItem();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	virtual void ProvideAttributes(TArray<FItemAttribute_Float> & AttributesArrayToSupply) {}

public:	
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;


	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns the localized name of this item. */
	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemName() const { return Name; }

	/** Returns the visual representation of this item as an icon. */
	UFUNCTION(BlueprintPure, Category = "Item")
	UTexture2D* GetItemIcon() const { return Icon; }

	/** Returns the rarity of this item. */
	UFUNCTION(BlueprintPure, Category = "Item")
	UItemRarity* GetItemRarity() const { return Rarity; }

	/** Returns all the attributes of this item that can be displayed in a card-type item info widget. */
	UFUNCTION(BlueprintCallable, Category = "Item | Attribute")
	TArray<FItemAttribute_Float> GetPrintableItemAttributes();


	/**********************************
				 SETTERS
	**********************************/

public:
	void SetItemName(FText & ItemName) { this->Name = ItemName; }
	void SetItemIcon(UTexture2D* ItemIcon) { this->Icon = ItemIcon; }
	void SetItemRarity(UItemRarity* ItemRarity) { this->Rarity = ItemRarity; }
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


USTRUCT(BlueprintType)
struct FItemAttribute
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	float Value;

	FItemAttribute()
	{}

	FItemAttribute(UTexture2D* _Icon, FText _Name, float _Value)
		: Icon(_Icon), Name(_Name), Value(_Value)
	{}
};

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

public:	
	/** Sets default values. */
	AItem();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	virtual void ProvideAttributes(TArray<FItemAttribute> & AttributesArrayToSupply) {}

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

	/** Returns all the attributes of this item that can be displayed in a card-type item info widget. */
	UFUNCTION(BlueprintCallable, Category = "Item | Attribute")
	TArray<FItemAttribute> GetPrintableItemAttributes();
};

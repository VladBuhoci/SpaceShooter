// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"


/**
 * Base class of all player-usable items (ammo, weapons etc).
 */
UCLASS()
class SPACESHOOTER_API AItem : public AActor
{
	GENERATED_BODY()

protected:
	/** Name of this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	FText Name;

	/** Icon of this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", Meta = (AllowPrivateAccess = "true"))
	UTexture2D* Icon;
	
public:	
	/** Sets default values. */
	AItem();

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
	/** Returns the localized name of this item. */
	UFUNCTION(BlueprintPure, Category = "Item")
	FText GetItemName() const { return Name; }

	/** Returns the visual representation of this item as an icon. */
	UFUNCTION(BlueprintPure, Category = "Item")
	UTexture2D* GetItemIcon() const { return Icon; }
};

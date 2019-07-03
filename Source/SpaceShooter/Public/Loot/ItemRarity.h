// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemRarity.generated.h"


/**
 * Provides a distinguished name and colour for a particular rarity type.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UItemRarity : public UObject
{
	GENERATED_BODY()
	
protected:
	/** Name of this rarity type. */
	UPROPERTY(EditDefaultsOnly, Category = "Loot | Item Rarity")
	FText RarityName;

	/** Color of this rarity type. It will be used to distinguish items in the game. */
	UPROPERTY(EditDefaultsOnly, Category = "Loot | Item Rarity")
	FColor RarityColour;

	/** Grade of this rarity type. It will be used to sort items in an array. */
	UPROPERTY(EditDefaultsOnly, Category = "Loot | Item Rarity")
	int32 RarityGrade;

public:
	UFUNCTION(BlueprintPure, Category = "Loot | Item Rarity")
	FText GetRarityName() const { return RarityName; }

	UFUNCTION(BlueprintPure, Category = "Loot | Item Rarity")
	FColor GetRarityColour() const { return RarityColour; }

	UFUNCTION(BlueprintPure, Category = "Loot | Item Rarity")
	int32 GetRarityGrade() const { return RarityGrade; }
};

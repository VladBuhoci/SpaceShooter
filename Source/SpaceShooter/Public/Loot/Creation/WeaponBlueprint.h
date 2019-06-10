// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Creation/ItemBlueprint.h"

#include "CoreMinimal.h"
#include "WeaponBlueprint.generated.h"

// Forward declarations.
class UTexture2D;


/**
 * Used by the LootWeaponBuilder class to instantiate Weapon objects.
 */
UCLASS(Blueprintable)
class SPACESHOOTER_API UWeaponBlueprint : public UItemBlueprint
{
	GENERATED_BODY()
	
private:
	/** Type of weapon to be spawned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Blueprint", Meta = (AllowPrivateAccess = "true"))
	EWeaponType Type;

public:
	FText GetItemName() const { return FText::FromString("A Gun"); }
	UTexture2D* GetItemIcon() const { return nullptr; }
	EWeaponType GetType() const { return Type; }
};

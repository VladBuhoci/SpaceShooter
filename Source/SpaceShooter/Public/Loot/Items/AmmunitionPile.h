// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Loot/Items/Item.h"
#include "Globals/SpaceEnums.h"

#include "CoreMinimal.h"
#include "AmmunitionPile.generated.h"


/**
 * Base class for ammo piles that can be picked up, partially or completely, depleting them of ammo units.
 */
UCLASS()
class SPACESHOOTER_API AAmmunitionPile : public AItem
{
	GENERATED_BODY()
	
private:
	/** Type of weapon this ammunition can be used for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Pile", Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponTypeAmmo;

	/** Amount of ammunition units this pile contains. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Pile", Meta = (AllowPrivateAccess = "true"))
	FItemAttribute_Float CurrentAmmoUnitsCount;

public:
	/** Sets default values. */
	AAmmunitionPile();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

	/**
	 * Takes ammo from the pile and returns that quantity or whatever is left inside.
	 */
	int32 TakeAmmo(int32 AmountToTake);


	/**********************************
				 GETTERS
	**********************************/

public:
	/** Returns true if this ammo pile is no longer able to supply a spacecraft. */
	bool IsEmpty() const { return GetAmmoAmmount() <= 0; }

	virtual void ProvideAttributes(TArray<FItemAttribute_Float> & AttributesArrayToSupply) override;

	/** Returns the type of weapon this ammo is used for. */
	UFUNCTION(BlueprintPure, Category = "Ammunition Pile")
	EWeaponType GetAmmoType() const { return WeaponTypeAmmo; }

	/** Returns the current ammo count of this pile. */
	UFUNCTION(BlueprintPure, Category = "Ammunition Pile")
	int32 GetAmmoAmmount() const { return CurrentAmmoUnitsCount.Value; }


	/**********************************
				 SETTERS
	**********************************/

public:
	void SetAmmoType(EWeaponType Type) { WeaponTypeAmmo = Type; }
	void SetAmmoCountAttribute(FItemAttribute_Float & AmmoAttr);
};

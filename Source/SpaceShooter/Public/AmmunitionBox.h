// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "SpaceEnums.h"
#include "AmmunitionBox.generated.h"


/**
 * Base class for ammo boxes that can be picked up, partially or completely depleting them of ammo units.
 */
UCLASS()
class SPACESHOOTER_API AAmmunitionBox : public AItem
{
	GENERATED_BODY()
	
private:
	/** Type of weapon this ammunition can be used for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Box", Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponTypeAmmo;

	/** Amount of ammunition units this box contains. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammunition Box", Meta = (AllowPrivateAccess = "true"))
	FItemAttribute TotalAmmoUnits;

public:
	/** Sets default values. */
	AAmmunitionBox();

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
	virtual bool IsEmpty() const override { return GetAmmoAmmount() <= 0; }

	virtual void ProvideAttributes(TArray<FItemAttribute> & AttributesArrayToSupply) override;

	/** Returns the type of weapon this ammo is used for. */
	UFUNCTION(BlueprintPure, Category = "Ammunition Box")
	EWeaponType GetAmmoType() const { return WeaponTypeAmmo; }

	/** Returns the total ammo count of this box. */
	UFUNCTION(BlueprintPure, Category = "Ammunition Box")
	int32 GetAmmoAmmount() const { return TotalAmmoUnits.Value; }

	/**
	 * Takes ammo from the box and returns that quantity or whatever is left inside.
	 */
	int32 TakeAmmo(int32 AmountToTake);
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "AmmunitionPile.h"


AAmmunitionPile::AAmmunitionPile()
{
	Name                  = FText::FromString("Unnamed Ammo Pile");
	WeaponTypeAmmo        = EWeaponType::Unknown;
	CurrentAmmoUnitsCount = FItemAttribute(NULL, FText::FromString("Ammo Count"), 0);
}

void AAmmunitionPile::BeginPlay()
{
	Super::BeginPlay();

}

void AAmmunitionPile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmunitionPile::ProvideAttributes(TArray<FItemAttribute> & AttributesArrayToSupply)
{
	AttributesArrayToSupply.Add(CurrentAmmoUnitsCount);
}

int32 AAmmunitionPile::TakeAmmo(int32 AmountToTake)
{
	if (AmountToTake == 0)
		return 0;

	int32 Delta              = FMath::Clamp((int) CurrentAmmoUnitsCount.Value - AmountToTake, 0, AmountToTake);
	int32 ActualAmountToTake = Delta == 0 ? CurrentAmmoUnitsCount.Value : Delta > AmountToTake ? Delta : AmountToTake;

	CurrentAmmoUnitsCount.Value -= ActualAmountToTake;

	return ActualAmountToTake;
}

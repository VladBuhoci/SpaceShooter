// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "AmmunitionBox.h"


AAmmunitionBox::AAmmunitionBox()
{
	Name           = FText::FromString("Unnamed Ammo Box");
	WeaponTypeAmmo = EWeaponType::Unknown;
	TotalAmmoUnits = FItemAttribute(NULL, FText::FromString("Ammo Count"), 0);
}

void AAmmunitionBox::BeginPlay()
{
	Super::BeginPlay();

}

void AAmmunitionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmunitionBox::ProvideAttributes(TArray<FItemAttribute> & AttributesArrayToSupply)
{
	AttributesArrayToSupply.Add(TotalAmmoUnits);
}

int32 AAmmunitionBox::TakeAmmo(int32 AmountToTake)
{
	if (AmountToTake == 0)
		return 0;

	int32 Delta = FMath::Clamp((int) TotalAmmoUnits.Value - AmountToTake, 0, AmountToTake);
	int32 ActualAmountToTake = Delta == 0 ? TotalAmmoUnits.Value : Delta > AmountToTake ? Delta : AmountToTake;

	TotalAmmoUnits.Value -= ActualAmountToTake;

	return ActualAmountToTake;
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "AmmunitionBox.h"


AAmmunitionBox::AAmmunitionBox()
{
	Name           = FText::FromString("Unnamed Ammo Box");
	WeaponTypeAmmo = EWeaponType::Unknown;
	TotalAmmoUnits = 0;
}

void AAmmunitionBox::BeginPlay()
{
	Super::BeginPlay();

}

void AAmmunitionBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

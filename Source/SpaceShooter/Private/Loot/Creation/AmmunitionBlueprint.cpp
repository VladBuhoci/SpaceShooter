// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Creation/AmmunitionBlueprint.h"


void UAmmunitionBlueprint::GetAmmoCountRandomized(FItemAttribute_Float & AmmoAttr) const
{
	AmmoAttr.Icon  = CurrentAmmoUnitsCount.Icon;
	AmmoAttr.Name  = CurrentAmmoUnitsCount.Name;
	AmmoAttr.Value = FMath::RandRange(CurrentAmmoUnitsCount.ValueLow, CurrentAmmoUnitsCount.ValueHigh);
}

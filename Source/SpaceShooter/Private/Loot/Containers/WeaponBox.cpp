// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Containers/WeaponBox.h"
#include "Loot/Items/Weapon.h"


AItem* AWeaponBox::GetContainedItem() const
{
	return ContainedItem;
}

void AWeaponBox::SetContainedItem(AItem* NewItem)
{
	if (AWeapon* NewWeapon = Cast<AWeapon>(NewItem))
	{
		ContainedItem = NewWeapon;
	}
}

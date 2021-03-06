// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Containers/AmmunitionBox.h"
#include "Loot/Items/AmmunitionPile.h"


AItem* AAmmunitionBox::GetContainedItem() const
{
	return ContainedItem;
}

void AAmmunitionBox::SetContainedItem(AItem* NewItem)
{
	if (AAmmunitionPile* NewAmmoPile = Cast<AAmmunitionPile>(NewItem))
	{
		ContainedItem = NewAmmoPile;
	}
}

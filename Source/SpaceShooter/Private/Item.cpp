// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Item.h"


AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Name = FText::FromString("Blank");
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FItemAttribute> AItem::GetPrintableItemAttributes()
{
	TArray<FItemAttribute> Attributes;

	ProvideAttributes(Attributes);

	return Attributes;
}

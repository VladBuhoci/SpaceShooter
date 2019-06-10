// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Containers/ItemBox.h"
#include "Loot/Items/Item.h"

#include "Components/SkeletalMeshComponent.h"


AItemBox::AItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Box Mesh Component");

	RootComponent = BoxMeshComponent;
}

void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

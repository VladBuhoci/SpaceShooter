// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootChest.h"
#include "XYOnlyPhysicsConstraintComponent.h"

#include "Components/SkeletalMeshComponent.h"


ALootChest::ALootChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestMeshComponent                = CreateDefaultSubobject<USkeletalMeshComponent>("Chest Mesh Component");
	XYPlanePhysicsConstraintComponent = CreateDefaultSubobject<UXYOnlyPhysicsConstraintComponent>("XY Plane Physics Constraint Component");

	RootComponent = ChestMeshComponent;

	ChestMeshComponent->SetSimulatePhysics(true);
	ChestMeshComponent->SetEnableGravity(false);
	ChestMeshComponent->SetCollisionProfileName("PhysicsActor");	// Use the PhysicsActor profile for collision setup
																	//		(collision channels and responses).

	XYPlanePhysicsConstraintComponent->SetupAttachment(RootComponent);
	XYPlanePhysicsConstraintComponent->SetActorConstrainedComponent(RootComponent);
}

void ALootChest::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALootChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


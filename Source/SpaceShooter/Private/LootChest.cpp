// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootChest.h"

#include "Components/SkeletalMeshComponent.h"

#include "PhysicsEngine/PhysicsConstraintComponent.h"


ALootChest::ALootChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestMesh                  = CreateDefaultSubobject<USkeletalMeshComponent>("Chest Mesh");
	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>("Physics Constraint Component");

	RootComponent = ChestMesh;

	ChestMesh->SetSimulatePhysics(true);
	ChestMesh->SetEnableGravity(false);
	ChestMesh->SetCollisionProfileName("PhysicsActor");

	PhysicsConstraintComponent->SetupAttachment(ChestMesh);
	PhysicsConstraintComponent->ComponentName1.ComponentName = "Chest Mesh";
	// Leave ComponentName2 NULL to constrain this actor to the world.
	PhysicsConstraintComponent->SetLinearXLimit(LCM_Free, 0.0f);			// Movement on X
	PhysicsConstraintComponent->SetLinearYLimit(LCM_Free, 0.0f);			// Movement on Y
	PhysicsConstraintComponent->SetLinearZLimit(LCM_Locked, 0.0f);			// Movement on Z
	PhysicsConstraintComponent->SetAngularTwistLimit(ACM_Locked, 0.0f);		// Rotation around X
	PhysicsConstraintComponent->SetAngularSwing2Limit(ACM_Locked, 0.0f);	// Rotation around Y
	PhysicsConstraintComponent->SetAngularSwing1Limit(ACM_Free, 0.0f);		// Rotation around Z
}

void ALootChest::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALootChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


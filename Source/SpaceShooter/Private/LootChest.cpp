// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootChest.h"
#include "XYOnlyPhysicsConstraintComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Runtime/Engine/Public/TimerManager.h"


ALootChest::ALootChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestMeshComponent                = CreateDefaultSubobject<USkeletalMeshComponent           >("Chest Mesh Component");
	XYPlanePhysicsConstraintComponent = CreateDefaultSubobject<UXYOnlyPhysicsConstraintComponent>("XY Plane Physics Constraint Component");
	InfoWidgetComponent               = CreateDefaultSubobject<UWidgetComponent                 >("Info Widget Component");

	ChestName            = FText::FromString("Unnamed Chest");
	CurrentState         = ELootChestState::Closed;
	TimeBeforePhysicsOff = 10.0f;

	RootComponent = ChestMeshComponent;

	ChestMeshComponent->SetSimulatePhysics(true);
	ChestMeshComponent->SetEnableGravity(false);
	ChestMeshComponent->SetCollisionProfileName("PhysicsActor");	// Use the PhysicsActor profile for collision setup
																	//		(collision channels and responses).

	XYPlanePhysicsConstraintComponent->SetupAttachment(RootComponent);
	XYPlanePhysicsConstraintComponent->SetActorConstrainedComponent(RootComponent);

	InfoWidgetComponent->SetupAttachment(RootComponent);
	InfoWidgetComponent->PrimaryComponentTick.bCanEverTick = true;	// false by default, no clue why, but it won't render the widget that way.
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);		// Draw using screen coordinates so the widget looks 2D, not 3D.
	InfoWidgetComponent->SetDrawSize(FVector2D(150.0f, 50.0f));		// Size of the rectangular surface the widget is drawn on.
	InfoWidgetComponent->SetPivot(FVector2D(0.5f, 1.5f));			// Offset from origin of this actor's local space.
	InfoWidgetComponent->SetVisibility(false);
}

void ALootChest::BeginPlay()
{
	Super::BeginPlay();
	
	// After a while, we want to stop simulating physics.
	FTimerHandle NoMorePhysicsSimulationTimerHandler;

	GetWorldTimerManager().SetTimer(NoMorePhysicsSimulationTimerHandler, [this]() {
		ChestMeshComponent->SetSimulatePhysics(false);
	}, TimeBeforePhysicsOff, false);
}

void ALootChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALootChest::OnMouseEnter_Implementation()
{
	InfoWidgetComponent->SetVisibility(true);
}

void ALootChest::OnMouseLeave_Implementation()
{
	InfoWidgetComponent->SetVisibility(false);
}


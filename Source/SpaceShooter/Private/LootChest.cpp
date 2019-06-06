// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "LootChest.h"
#include "XYOnlyPhysicsConstraintComponent.h"
#include "LootPool.h"
#include "LootItemReceiver.h"
#include "ItemBox.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

#include "Animation/AnimSequence.h"

#include "Runtime/Engine/Public/TimerManager.h"


ALootChest::ALootChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestMeshComponent                = CreateDefaultSubobject<USkeletalMeshComponent           >("Chest Mesh Component");
	XYPlanePhysicsConstraintComponent = CreateDefaultSubobject<UXYOnlyPhysicsConstraintComponent>("XY Plane Physics Constraint Component");
	InfoWidgetComponent               = CreateDefaultSubobject<UWidgetComponent                 >("Info Widget Component");
	ChestOverviewWidgetComponent      = CreateDefaultSubobject<UWidgetComponent                 >("Treasure Chest Overview Widget Component");

	ChestName                  = FText::FromString("Unnamed Chest");
	CurrentState               = ELootChestState::Closed;
	TimeBeforePhysicsOff       = 10.0f;
	bCurrentlyPointedAt        = false;
	bCurrentlyBeingInspected   = false;
	CurrentlySelectedItemBoxIndex = 0;

	RootComponent = ChestMeshComponent;

	BeginPhysicsSimulation();

	XYPlanePhysicsConstraintComponent->SetupAttachment(RootComponent);
	XYPlanePhysicsConstraintComponent->SetActorConstrainedComponent(RootComponent);

	InfoWidgetComponent->SetupAttachment(RootComponent);
	InfoWidgetComponent->PrimaryComponentTick.bCanEverTick = true;	// false by default, no clue why, but it won't render the widget that way.
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);		// Draw using screen coordinates so the widget looks 2D, not 3D.
	InfoWidgetComponent->SetDrawSize(FVector2D(150.0f, 50.0f));		// Size of the rectangular surface the widget is drawn on.
	InfoWidgetComponent->SetPivot(FVector2D(0.5f, 1.5f));			// Offset from origin of this actor's local space.
	InfoWidgetComponent->SetVisibility(false);

	ChestOverviewWidgetComponent->SetupAttachment(RootComponent);
	ChestOverviewWidgetComponent->PrimaryComponentTick.bCanEverTick = true;	// false by default, no clue why, but it won't render the widget that way.
	ChestOverviewWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);		// Draw using screen coordinates so the widget looks 2D, not 3D.
	ChestOverviewWidgetComponent->SetDrawSize(FVector2D(200.0f, 100.0f));	// Size of the rectangular surface the widget is drawn on.
	ChestOverviewWidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));			// Offset from origin of this actor's local space.
	ChestOverviewWidgetComponent->SetVisibility(false);
}

void ALootChest::BeginPlay()
{
	Super::BeginPlay();
	
	// After a while, we want to stop simulating physics.
	FTimerHandle NoMorePhysicsSimulationTimerHandler;

	GetWorldTimerManager().SetTimer(NoMorePhysicsSimulationTimerHandler, [this]() {
		EndPhysicsSimulation();
	}, TimeBeforePhysicsOff, false);
}

void ALootChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALootChest::BeginPhysicsSimulation()
{
	ChestMeshComponent->SetSimulatePhysics(true);
	ChestMeshComponent->SetEnableGravity(false);
	ChestMeshComponent->SetCollisionProfileName("PhysicsActor");	// Use the PhysicsActor profile for collision setup
																	//		(collision channels and responses).
}

void ALootChest::EndPhysicsSimulation()
{
	ChestMeshComponent->SetSimulatePhysics(false);
}

void ALootChest::OnMouseEnter_Implementation()
{
	bCurrentlyPointedAt = true;

	PresentChestIdentity();
}

void ALootChest::OnMouseLeave_Implementation()
{
	bCurrentlyPointedAt = false;

	HideChestIdentity();

	// Make sure we end chest item inspection, in case it was active.
	EndChestInspection();
}

void ALootChest::Interact(ILootItemReceiver* ReceivingPawn)
{
	if (ReceivingPawn == nullptr)
		return;

	HideChestIdentity();

	if (CurrentState == ELootChestState::Opened)
	{
		// If the chest is already opened, this current interaction means one of two things:
		//		either enable the widget responsible with displaying all item boxes in the
		//		chest or grab the currently selected box's item from the aforementioned widget.

		if (!bCurrentlyBeingInspected)
		{
			BeginChestInspection();
		}
		else
		{
			GrabHighlightedItemBoxFromChest(ReceivingPawn);
		}
	}
	else if (CurrentState == ELootChestState::Closed)
	{
		GenerateItemsAndItemBoxes();

		// The blueprint implementation will send the array data to the Treasure Chest Overview widget.
		OnChestPreOpen();
		
		OpenChest();
	}
}

void ALootChest::HighlightPreviousItemBoxInsideChest()
{
	if (CurrentState == ELootChestState::Closed && !bCurrentlyBeingInspected)
		return;

	if (AreItemsLeft() && CurrentlySelectedItemBoxIndex > 0)
	{
		CurrentlySelectedItemBoxIndex -= 1;

		OnHighlightPreviousItemBoxInsideChest();
	}
}

void ALootChest::HighlightNextItemBoxInsideChest()
{
	if (CurrentState == ELootChestState::Closed && !bCurrentlyBeingInspected)
		return;

	if (AreItemsLeft() && CurrentlySelectedItemBoxIndex < ContainedItemBoxes.Num() - 1)
	{
		CurrentlySelectedItemBoxIndex += 1;

		OnHighlightNextItemBoxInsideChest();
	}
}

void ALootChest::PresentChestIdentity()
{
	InfoWidgetComponent->SetVisibility(true);
}

void ALootChest::HideChestIdentity()
{
	InfoWidgetComponent->SetVisibility(false);
}

void ALootChest::BeginChestInspection()
{
	bCurrentlyBeingInspected = true;

	ChestOverviewWidgetComponent->SetVisibility(true);
}

void ALootChest::EndChestInspection()
{
	bCurrentlyBeingInspected = false;

	ChestOverviewWidgetComponent->SetVisibility(false);

	// Go back to zero to be prepared for the next time when inspection is activated.
	CurrentlySelectedItemBoxIndex = 0;
}

void ALootChest::GenerateItemsAndItemBoxes()
{
	UItemPoolListDefinition* ItemPoolListDef = NewObject<UItemPoolListDefinition>(this, LootDefinitionClass);

	ContainedItemBoxes = ItemPoolListDef->GetRandomItemsWrappedInBoxes();
}

void ALootChest::OpenChest()
{
	CurrentState = ELootChestState::Opening;

	// Playing an opening animation (but only after stopping physics simulation, otherwise it won't play).

	GetWorldTimerManager().ClearAllTimersForObject(this);
	EndPhysicsSimulation();

	if (OpenAnimation != NULL)
	{
		ChestMeshComponent->PlayAnimation(OpenAnimation, false);

		float AnimationLength = OpenAnimation->SequenceLength / OpenAnimation->RateScale;

		// Wait for the animation to play until the end before changing the state to "Opened".

		FTimerHandle ChestOpeningTimerHandle;

		GetWorldTimerManager().SetTimer(ChestOpeningTimerHandle, [this]() {
			CurrentState = ELootChestState::Opened;

			// Make sure the Items Overview widget does not show up on top of the Identity widget.
			HideChestIdentity();

			// If the chest is still being pointed at...
			if (bCurrentlyPointedAt)
			{
				// Make things easier for the player by automatically displaying
				//		the item info widget after opening this chest.
				BeginChestInspection();
			}
		}, AnimationLength, false);
	}
}

void ALootChest::GrabHighlightedItemBoxFromChest(ILootItemReceiver* ReceivingPawn)
{
	AItemBox* GrabbedItemBox = NULL;
	UObject* Receiver = Cast<UObject>(ReceivingPawn);

	if (Receiver && AreItemsLeft())
	{
		int32 PreviousHighlightedItemIndex = CurrentlySelectedItemBoxIndex;

		// Get a reference to the item box of interest.
		GrabbedItemBox = ContainedItemBoxes[PreviousHighlightedItemIndex];

		if (GrabbedItemBox)
		{
			bool bItemTaken = false;

			// Provide the actual item to the receiver.
			ILootItemReceiver::Execute_Supply(Receiver, GrabbedItemBox->GetContainedItem(), bItemTaken);

			if (bItemTaken)
			{
				// Remove the item box from the chest's array.
				ContainedItemBoxes.Remove(GrabbedItemBox);

				// Calculate the index for the next highlighted item box because there must always be a highlighted box...
				if (CurrentlySelectedItemBoxIndex >= ContainedItemBoxes.Num())
				{
					// ... but only change the index if its current value is not in the array's bounds anymore.
					CurrentlySelectedItemBoxIndex = ContainedItemBoxes.Num() - 1;
				}
			}

			// The blueprint implementation of this method will communicate with the Treasure Chest Overview widget
			//		to maintain synchronization between the chest and the HUD.
			OnGrabHighlightedItemBoxFromChest(PreviousHighlightedItemIndex, bItemTaken);

			if (bItemTaken)
			{
				GrabbedItemBox->Destroy();
			}
		}
	}
}

bool ALootChest::AreItemsLeft() const
{
	return ContainedItemBoxes.Num() > 0;
}

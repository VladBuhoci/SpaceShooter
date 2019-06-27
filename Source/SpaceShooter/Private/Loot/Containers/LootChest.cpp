// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Containers/LootChest.h"
#include "Loot/Containers/ItemBox.h"
#include "Loot/Creation/LootPool.h"
#include "Loot/Items/LootItemReceiver.h"
#include "Components/XYOnlyPhysicsConstraintComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"

#include "Animation/AnimSequence.h"

#include "Runtime/Engine/Public/TimerManager.h"


ALootChest::ALootChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ChestMeshComponent                = CreateDefaultSubobject<USkeletalMeshComponent           >("Chest Mesh Component");
	PointLightComponent               = CreateDefaultSubobject<UPointLightComponent             >("Point Light Component");
	XYPlanePhysicsConstraintComponent = CreateDefaultSubobject<UXYOnlyPhysicsConstraintComponent>("XY Plane Physics Constraint Component");
	InfoWidgetComponent               = CreateDefaultSubobject<UWidgetComponent                 >("Info Widget Component");
	ChestOverviewWidgetComponent      = CreateDefaultSubobject<UWidgetComponent                 >("Loot Chest Overview Widget Component");

	ChestName                     = FText::FromString("Unnamed Chest");
	CurrentState                  = ELootChestState::Closed;
	LightIntensityLow             = 10.0f;
	LightIntensityNormal          = 1000.0f;
	LightIntensityHigh            = 10000.0f;
	LightIntensitySwapSpeed       = 35000.0f;
	LightIntensitySwapInterval    = 1.0f;
	bCurrentlyPointedAt           = false;
	bCurrentlyBeingInspected      = false;
	CurrentlySelectedItemBoxIndex = 0;

	RootComponent = ChestMeshComponent;

	BeginPhysicsSimulation();

	ChestMeshComponent->bGenerateOverlapEvents = true;

	PointLightComponent->SetupAttachment(RootComponent);
	PointLightComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	PointLightComponent->SetAttenuationRadius(150.0f);
	PointLightComponent->bUseInverseSquaredFalloff = false;

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

	LightIntensityCurrentTargetValue = LightIntensityNormal;
	
	// Visual effect for this loot chest's birth: high light intensity level to make it shine more than the others.
	SetLightIntensityLevel(LightIntensityHigh, LightIntensityNormal);
}

void ALootChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleLightIntensity(DeltaTime);
}

void ALootChest::HandleLightIntensity(float DeltaTime)
{
	if (FMath::IsNearlyEqual(PointLightComponent->Intensity, LightIntensityCurrentTargetValue, 0.0f))
		return;

	float NewIntensityLevel = FMath::FInterpConstantTo(PointLightComponent->Intensity, LightIntensityCurrentTargetValue,
		DeltaTime, LightIntensitySwapSpeed);

	PointLightComponent->SetIntensity(NewIntensityLevel);
}

void ALootChest::SetLightIntensityLevel(float NewIntensityTargetValue)
{
	GetWorldTimerManager().ClearTimer(LightIntensityTimerHandle);

	LightIntensityCurrentTargetValue = NewIntensityTargetValue;
}

void ALootChest::SetLightIntensityLevel(float NewTemporaryIntensityTargetValue, float FuturePermanentIntensityTargetValue)
{
	GetWorldTimerManager().ClearTimer(LightIntensityTimerHandle);

	// Force-set the intensity level to the previous value so the interpolation will have a visual impact after this.
	// Useful when the intensity is in a transitory state (between Normal and High).
	PointLightComponent->SetIntensity(FuturePermanentIntensityTargetValue);

	LightIntensityCurrentTargetValue = NewTemporaryIntensityTargetValue;

	GetWorldTimerManager().SetTimer(LightIntensityTimerHandle, [this, FuturePermanentIntensityTargetValue]() {
		LightIntensityCurrentTargetValue = FuturePermanentIntensityTargetValue;
	}, LightIntensitySwapInterval, false);
}

void ALootChest::BeginPhysicsSimulation()
{
	if (ChestMeshComponent)
	{
		ChestMeshComponent->SetSimulatePhysics(true);
		ChestMeshComponent->SetEnableGravity(false);
		ChestMeshComponent->SetCollisionProfileName("PhysicsActor");	// Use the PhysicsActor profile for collision setup
																		//		(collision channels and responses).
	}
}

void ALootChest::EndPhysicsSimulation()
{
	if (ChestMeshComponent)
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

		if (!bCurrentlyBeingInspected && HasAnyItemBoxesLeft())
		{
			BeginChestInspection();
		}
		else
		{
			GrabItemBoxFromChest(ReceivingPawn, CurrentlySelectedItemBoxIndex);
		}
	}
	else if (CurrentState == ELootChestState::Closed)
	{
		PopulateAndOpenChest();
	}
}

void ALootChest::PickUpItemsOfTypes(ILootItemReceiver* ReceivingPawn, TArray<TSubclassOf<AItemBox>> ItemBoxTypes)
{
	float TimeToWaitBeforeChestIsOpened = 0.0f;

	// In this case, we do not care if the chest is being inspected at the moment or not, only if it is opened.
	// This is because the method will most likely be called by an auto-picking method on the ReceivingPawn's side.

	if (CurrentState == ELootChestState::Closed)
	{
		PopulateAndOpenChest();

		// Add a delay to the waiting time, just in case there's an overhead somewhere.
		TimeToWaitBeforeChestIsOpened = GetOpenAnimationLength() + 0.2f;
	}

	if (TimeToWaitBeforeChestIsOpened > 0.0f)
	{
		FTimerHandle PickUpItemsTimerHandle;

		GetWorldTimerManager().SetTimer(PickUpItemsTimerHandle, [this, ReceivingPawn, ItemBoxTypes]() {
			PickUpItemsOfTypes_Internal(ReceivingPawn, ItemBoxTypes);
		}, TimeToWaitBeforeChestIsOpened, false);
	}
	else
	{
		PickUpItemsOfTypes_Internal(ReceivingPawn, ItemBoxTypes);
	}
}

void ALootChest::PickUpItemsOfTypes_Internal(ILootItemReceiver* ReceivingPawn, TArray<TSubclassOf<AItemBox>> ItemBoxTypes)
{
	if (CurrentState == ELootChestState::Opened && HasAnyItemBoxesLeft())
	{
		// Acquire the index of every item that has one of the given types.
		TArray<int32> ItemBoxIndices;

		for (int32 i = 0; i < ContainedItemBoxes.Num(); i++)
		{
			bool bIsOfChosenType = false;

			for (UClass* Clazz : ItemBoxTypes)
			{
				if (ContainedItemBoxes[i]->GetClass()->IsChildOf(Clazz))
				{
					bIsOfChosenType = true;
					break;
				}
			}

			if (bIsOfChosenType)
			{
				ItemBoxIndices.Add(i);
			}
		}

		// If we found any valid item box indices, begin picking them up.
		if (ItemBoxIndices.Num() > 0)
		{
			GrabItemBoxesFromChest(ReceivingPawn, ItemBoxIndices);
		}
	}
}

void ALootChest::HighlightPreviousItemBoxInsideChest()
{
	if (CurrentState == ELootChestState::Closed && !bCurrentlyBeingInspected)
		return;

	if (HasAnyItemBoxesLeft() && CurrentlySelectedItemBoxIndex > 0)
	{
		CurrentlySelectedItemBoxIndex -= 1;

		OnHighlightPreviousItemBoxInsideChest();
	}
}

void ALootChest::HighlightNextItemBoxInsideChest()
{
	if (CurrentState == ELootChestState::Closed && !bCurrentlyBeingInspected)
		return;

	if (HasAnyItemBoxesLeft() && CurrentlySelectedItemBoxIndex < ContainedItemBoxes.Num() - 1)
	{
		CurrentlySelectedItemBoxIndex += 1;

		OnHighlightNextItemBoxInsideChest();
	}
}

void ALootChest::PresentChestIdentity()
{
	if (InfoWidgetComponent && !InfoWidgetComponent->IsVisible())
		InfoWidgetComponent->SetVisibility(true);
}

void ALootChest::HideChestIdentity()
{
	if (InfoWidgetComponent && InfoWidgetComponent->IsVisible())
		InfoWidgetComponent->SetVisibility(false);
}

void ALootChest::BeginChestInspection()
{
	if (!bCurrentlyBeingInspected)
	{
		bCurrentlyBeingInspected = true;

		if (ChestOverviewWidgetComponent)
			ChestOverviewWidgetComponent->SetVisibility(true);
	}
}

void ALootChest::EndChestInspection()
{
	if (bCurrentlyBeingInspected)
	{
		bCurrentlyBeingInspected = false;

		if (ChestOverviewWidgetComponent)
			ChestOverviewWidgetComponent->SetVisibility(false);

		// Go back to zero to be prepared for the next time when inspection is activated.
		CurrentlySelectedItemBoxIndex = 0;
	}
}

void ALootChest::PopulateAndOpenChest()
{
	GenerateItemsAndItemBoxes();

	// The blueprint implementation will send the array data to the Loot Chest Overview widget.
	OnChestPreOpen();

	OpenChest();
}

void ALootChest::GenerateItemsAndItemBoxes()
{
	if (LootDefinitionClass)
	{
		UItemPoolListDefinition* ItemPoolListDef = NewObject<UItemPoolListDefinition>(this, LootDefinitionClass);

		ContainedItemBoxes = ItemPoolListDef->GetRandomItemsWrappedInBoxes();
	}
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

		float AnimationLength = GetOpenAnimationLength();

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

			// If there are no items generated in this chest, close the inspection widget and change the light intensity.
			if (!HasAnyItemBoxesLeft())
			{
				EndChestInspection();
				SetLightIntensityLevel(LightIntensityLow);
				OnChestEmptied();
			}
		}, AnimationLength, false);
	}
}

void ALootChest::GrabItemBoxFromChest(ILootItemReceiver* ReceivingPawn, int32 ChosenItemBoxIndex)
{
	TArray<int32> SingleIndexArray;
	
	SingleIndexArray.Add(ChosenItemBoxIndex);

	GrabItemBoxesFromChest(ReceivingPawn, SingleIndexArray);
}

void ALootChest::GrabItemBoxesFromChest(ILootItemReceiver* ReceivingPawn, TArray<int32> ChosenItemBoxIndices)
{
	UObject* Receiver = Cast<UObject>(ReceivingPawn);

	if (Receiver && HasAnyItemBoxesLeft())
	{
		bool bSomethingWasTakenFromChest = false;
		int32 IndexReducingFactor = 0;

		for (int32 ChosenItemBoxIndex : ChosenItemBoxIndices)
		{
			EItemTakingAction ItemTakeAction = EItemTakingAction::None;

			GrabItemBoxFromChest_Internal(Receiver, ChosenItemBoxIndex - IndexReducingFactor, ItemTakeAction);

			// Increment the reducing factor value that will affect those indices left in the ChosenItemBoxIndices
			//	array, since removing any item box from the loot chest will decrement the indices of all the
			//	remaining item boxes, rendering the indices in this ChosenItemBoxIndices array incorrect.
			if (ItemTakeAction == EItemTakingAction::FullyTaken)
			{
				IndexReducingFactor++;
			}

			// Remember if anything has ever been taken from the chest during this single method call.
			if (ItemTakeAction == EItemTakingAction::FullyTaken || ItemTakeAction == EItemTakingAction::PartiallyTaken)
			{
				bSomethingWasTakenFromChest = true;
			}
		}

		// Trigger some visual effect whenever something is taken from the chest.
		if (bSomethingWasTakenFromChest)
		{
			if (!HasAnyItemBoxesLeft())
			{
				EndChestInspection();

				// Give Blueprint implementations a chance to react to this.
				OnChestEmptied();

				SetLightIntensityLevel(LightIntensityHigh, LightIntensityLow);
			}
			else
			{
				SetLightIntensityLevel(LightIntensityHigh, LightIntensityNormal);
			}
		}
	}
}

void ALootChest::GrabItemBoxFromChest_Internal(UObject* Receiver, int32 ChosenItemBoxIndex, EItemTakingAction & ItemTakingAction)
{
	int32 PreviousHighlightedItemIndex = ChosenItemBoxIndex;

	// Get a reference to the item box of interest.
	AItemBox* GrabbedItemBox = ContainedItemBoxes[PreviousHighlightedItemIndex];

	if (GrabbedItemBox)
	{
		// Provide the actual item to the receiver.
		ILootItemReceiver::Execute_Supply(Receiver, GrabbedItemBox->GetContainedItem(), ItemTakingAction);

		if (ItemTakingAction == EItemTakingAction::FullyTaken)
		{
			// Remove the item box from the chest's array.
			ContainedItemBoxes.Remove(GrabbedItemBox);

			// Calculate the index for the next highlighted item box because there must always be a highlighted box...
			if (CurrentlySelectedItemBoxIndex >= ContainedItemBoxes.Num())
			{
				// ... but only change the index if its current value is not within the array's bounds anymore.
				CurrentlySelectedItemBoxIndex = ContainedItemBoxes.Num() - 1;
			}

			GrabbedItemBox->Destroy();
		}

		// The blueprint implementation of this method will communicate with the Loot Chest Overview widget
		//		to maintain synchronization between the chest and the HUD.
		OnGrabHighlightedItemBoxFromChest(PreviousHighlightedItemIndex, ItemTakingAction == EItemTakingAction::FullyTaken);
	}
}

bool ALootChest::HasAnyItemBoxesLeft() const
{
	return ContainedItemBoxes.Num() > 0;
}

float ALootChest::GetOpenAnimationLength() const
{
	return OpenAnimation ? OpenAnimation->SequenceLength / OpenAnimation->RateScale : 0.0f;
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Pawns/SpacePlayerPawn.h"
#include "Loot/Inventory.h"
#include "Loot/Items/Weapon.h"
#include "Loot/Items/AmmunitionPile.h"
#include "Loot/Containers/LootChest.h"
#include "Loot/Containers/AmmunitionBox.h"
#include "Projectiles/Projectile.h"
#include "Effects/SpacecraftTurboModeCameraShake.h"
#include "Effects/SpacecraftDestructionCameraShake.h"
#include "Controllers/SpacePlayerController.h"
#include "UI/SpaceHUD.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Camera/CameraComponent.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"


/** Sets default values. */
ASpacePlayerPawn::ASpacePlayerPawn()
{
	// Initialize components.
	CentralSceneComponent                      = CreateDefaultSubobject<USceneComponent    >("Central Player Scene Component");
	CameraComponent                            = CreateDefaultSubobject<UCameraComponent   >("Camera Component");
	SpringArmComponent                         = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	LootChestQuickInteractArea                 = CreateDefaultSubobject<USphereComponent   >("Loot Chest Auto Interact Area");
	Inventory                                  = CreateDefaultSubobject<UInventory         >("Inventory");

	MoveForwardMaxTurboSpeed                   = 2400.0f;
	MoveForwardMaxSpeed                        = 1200.0f;
	MoveBackwardSpeed                          = 900.0f;
	MaxHitPoints                               = 250.0f;
	MaxShieldPoints                            = 500.0f;
	ShieldRechargeRate                         = 50.0f;
	ShieldRechargeDelay                        = 1.25f;
	SpacecraftTurnSpeed                        = 10.0f;
	SpringArmLength_SpeedFactor                = 0.25f;	// 25% of the spacecraft's velocity is used as base value for the spring arm's length.
	SpringArmLength_NormalFlightModeMultiplier = 1.0f;
	SpringArmLength_TurboFlightModeMultiplier  = 1.5f;
	CameraNormalFlightFOV                      = 90.0f;
	CameraTurboFlightFOV                       = 100.0f;
	CameraZoomSpeed                            = 10.0f;
	CameraFOVZoomSpeed                         = 5.0f;
	Name                                       = FText::FromString("Unnamed Player");
	Faction                                    = ESpacecraftFaction::Human;
	bCanInteract                               = true;
	InventorySlotsCount                        = 26;

	// CentralSceneComponent setup:
	CentralSceneComponent->SetupAttachment(SpacecraftMeshComponent);
	// ~ end of CentralSceneComponent setup.

	// SpringArmComponent setup:
	SpringArmComponent->SetupAttachment(CentralSceneComponent);
	SpringArmComponent->SetRelativeLocation(FVector(-850.0f, 0.0f, 900.0f));
	SpringArmComponent->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength          = 0.0f;
	SpringArmComponent->bEnableCameraLag         = true;					// Enable spring arm lag.
	SpringArmComponent->bEnableCameraRotationLag = false;					// Disable spring arm rotation lag. (camera never rotates)
	SpringArmComponent->CameraLagSpeed           = 20.0f;
	SpringArmComponent->bDoCollisionTest         = false;
	// ~ end of SpringArmComponent setup.

	// CameraComponent setup:
	CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);
	CameraComponent->bUsePawnControlRotation     = false;					// Do not rotate the camera along with the player.
	// ~ end of CameraComponent setup.

	// LootChestDetectionArea setup:
	LootChestQuickInteractArea->SetupAttachment(CentralSceneComponent);
	LootChestQuickInteractArea->SetSphereRadius(570.0f);
	LootChestQuickInteractArea->OnComponentBeginOverlap.AddDynamic(this, &ASpacePlayerPawn::OnLootChestEnterAutoInteractArea);
	// ~ end of LootChestDetectionArea setup.

	// Ammunition stocks:
	AmmoPools.Add(EWeaponType::Blaster , FAmmunitionStock(128, 256));
	AmmoPools.Add(EWeaponType::Cannon  , FAmmunitionStock(256, 512));
	AmmoPools.Add(EWeaponType::Volley  , FAmmunitionStock( 64, 128));
	AmmoPools.Add(EWeaponType::Launcher, FAmmunitionStock( 16,  32));

	// Inventory setup:
	Inventory->SetUpInventorySlots(InventorySlotsCount);
	// ~ end of Inventory setup.
}

/** Called when the game starts or when spawned. */
void ASpacePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	OnInventoryStateChanged();
}

/** Called every frame. */
void ASpacePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the helper scene component's rotation at a fixed zero value on all axis so the camera's
	//		 spring arm (that is attached to it) never rotates its children (i.e. the camera).
	CentralSceneComponent->SetWorldRotation(FRotator::ZeroRotator);
	
	CheckCameraOffset(DeltaTime);
}

void ASpacePlayerPawn::SearchAndCollectNearbySupplies()
{
	if (CanInteract())
		NearbyLootChestsCheckUp();
}

void ASpacePlayerPawn::OnLootChestEnterAutoInteractArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor != NULL && OtherActor != this && CanInteract())
	{
		ALootChest* FoundChest = Cast<ALootChest>(OtherActor);

		if (FoundChest)
		{
			PickUpItemsFromLootChest(FoundChest);
		}
	}
}

void ASpacePlayerPawn::NearbyLootChestsCheckUp()
{
	TArray<ALootChest*> FoundChests;

	LookForNearbyLootChestsForAutomaticPickUp(FoundChests);

	if (FoundChests.Num() > 0)
		for (auto Chest : FoundChests)
			PickUpItemsFromLootChest(Chest);
}

void ASpacePlayerPawn::LookForNearbyLootChestsForAutomaticPickUp(TArray<ALootChest*> & NearbyChests)
{
	TArray<AActor*> OverlappingActors;
	AActor* CurrentlyInspectedActor;
	ASpacePlayerController* SpacePlayerController = Cast<ASpacePlayerController>(GetController());

	if (!SpacePlayerController)
		return;

	CurrentlyInspectedActor = SpacePlayerController->GetCurrentMouseListeningActorPointedAt();

	LootChestQuickInteractArea->GetOverlappingActors(OverlappingActors, ALootChest::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		ALootChest* OverlappingChest = Cast<ALootChest>(OverlappingActor);

		// Only add this to the nearby chests array if it is not the currently inspected chest,
		//	but only if such a chest is inspected at that time.
		if (OverlappingChest && OverlappingChest != CurrentlyInspectedActor)
			NearbyChests.Add(OverlappingChest);
	}
}

void ASpacePlayerPawn::PickUpItemsFromLootChest(ALootChest* ChestToLoot)
{
	ChestToLoot->PickUpItemsOfTypes(this, TypesToPickUp);
}

void ASpacePlayerPawn::OnTurboModeActivated()
{
	// Play camera shake effect.

	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::PlayWorldCameraShake(World, USpacecraftTurboModeCameraShake::StaticClass(), GetActorLocation(), 0.0f, 10000.0f, 0.0f);
	}
}

void ASpacePlayerPawn::OnTurboModeDeactivated()
{
	
}

void ASpacePlayerPawn::OnDamageTaken(ASpacecraftPawn* DamageCauser)
{
	// TODO: add some hit effects perhaps?

}

void ASpacePlayerPawn::PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed)
{
	UWorld* World = GetWorld();

	if (World)
	{
		// Play camera shake effect.
		UGameplayStatics::PlayWorldCameraShake(World, USpacecraftDestructionCameraShake::StaticClass(), GetActorLocation(), 0.0f, 10000.0f, 0.0f);

		// Simulate slow motion.
 		UGameplayStatics::SetGlobalTimeDilation(World, 0.25f);
 		UGameplayStatics::SetGlobalPitchModulation(World, 0.75f, 1.0f);

		// Calling these time altering methods implies scheduling a call to a method to
		//		reset these global values after a bit of time.
		FTimerHandle DisableSlowmotionTimerHandle;

		GetWorldTimerManager().SetTimer(DisableSlowmotionTimerHandle, [World]() {
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			UGameplayStatics::SetGlobalPitchModulation(World, 1.0f, 1.0f);
		}, 1.0f, false);
	}

	// Make the player's ship invisible.
	SpacecraftMeshComponent->SetVisibility(false, true);

	if (ASpacePlayerController* MyController = Cast<ASpacePlayerController>(GetController()))
	{
		MyController->OnPlayerDied();
	}

	bShouldPlayDestroyEffects = true;
	bShouldBeDestroyed        = false;
}

void ASpacePlayerPawn::BeginFiringWeapon()
{
	Super::BeginFiringWeapon();

}

void ASpacePlayerPawn::EndFiringWeapon()
{
	Super::EndFiringWeapon();

}

void ASpacePlayerPawn::GetAllItemsFromInventory(TArray<AItem*> & Items)
{
	if (!Inventory)
		return;

	Inventory->GetAllItems(Items);
}

AItem* ASpacePlayerPawn::GetItemFromInventoryAt(int32 Index)
{
	return Inventory ? Inventory->GetItemAt(Index) : nullptr;
}

void ASpacePlayerPawn::SwapActiveWeaponWithActiveWeapon(int32 ActiveSlotIndex1, int32 ActiveSlotIndex2)
{
	if (ActiveSlotIndex1 < 1 || ActiveSlotIndex1 > 4 || ActiveSlotIndex2 < 1 || ActiveSlotIndex2 > 4)
		return;

	AWeapon* PreviousActiveWeapon1 = GetWeaponOnPreparedSlot(ActiveSlotIndex1);
	AWeapon* PreviousActiveWeapon2 = GetWeaponOnPreparedSlot(ActiveSlotIndex2);
	bool bWeapon1WasEquipped       = PreviousActiveWeapon1 == EquippedWeapon;
	bool bWeapon2WasEquipped       = PreviousActiveWeapon2 == EquippedWeapon;
	
	if (bWeapon1WasEquipped || bWeapon2WasEquipped)
		UnequipCurrentWeapon();

	SetWeaponOnPreparedSlot(PreviousActiveWeapon2, ActiveSlotIndex1, bWeapon1WasEquipped);
	SetWeaponOnPreparedSlot(PreviousActiveWeapon1, ActiveSlotIndex2, bWeapon2WasEquipped);

	OnActiveWeaponSlotsStateChanged();
}

void ASpacePlayerPawn::SwapActiveWeaponWithInventoryWeapon(int32 ActiveSlotIndex, int32 InventorySlotIndex)
{
	if (ActiveSlotIndex < 1 || ActiveSlotIndex > 4 || InventorySlotIndex < 0 || InventorySlotIndex > InventorySlotsCount - 1)
		return;

	if (Inventory)
	{
		AWeapon* PreviousActiveWeapon  = GetWeaponOnPreparedSlot(ActiveSlotIndex);
		AWeapon* PreviousInventWeapon  = Inventory->ReplaceItemAtIndex(InventorySlotIndex, PreviousActiveWeapon, true);
		bool bWeaponToSpareWasEquipped = PreviousActiveWeapon == EquippedWeapon;

		if (bWeaponToSpareWasEquipped)
			UnequipCurrentWeapon();

		// Do not receive OnOverheated/OnCooledDown calls from inventory weapons.
		PreviousActiveWeapon->RegisterStateListener(nullptr);

		SetWeaponOnPreparedSlot(PreviousInventWeapon, ActiveSlotIndex, bWeaponToSpareWasEquipped);

		OnActiveWeaponSlotsStateChanged();
		OnInventoryStateChanged();
	}
}

void ASpacePlayerPawn::RemoveItemFromInventory(int32 SlotIndex)
{
	if (Inventory)
	{
		if (Inventory->RemoveItem(SlotIndex))
		{
			OnInventoryStateChanged();
		}
	}
}

bool ASpacePlayerPawn::IsSpaceAvailableOnSpacecraft()
{
	return IsSpaceAvailableForAnotherWeapon() || IsSpaceAvailableInInventory();
}

bool ASpacePlayerPawn::IsSpaceAvailableInInventory()
{
	return Inventory ? !Inventory->IsFull() : false;
}

void ASpacePlayerPawn::AddItemToInventory(AItem* NewItem)
{
	if (Inventory)
	{
		if (Inventory->AddItem(NewItem, true))
		{
			OnInventoryStateChanged();
		}
	}
}

void ASpacePlayerPawn::OnActiveWeaponSlotsStateChanged()
{
	ASpacePlayerController* SpacePlayerController = Cast<ASpacePlayerController>(GetController());
	if (SpacePlayerController)
	{
		ASpaceHUD* SpaceHUD = Cast<ASpaceHUD>(SpacePlayerController->GetHUD());
		if (SpaceHUD)
		{
			SpaceHUD->OnPlayerSpacecraftActiveSlotsStateChanged();
		}
	}
}

void ASpacePlayerPawn::OnInventoryStateChanged()
{
	ASpacePlayerController* SpacePlayerController = Cast<ASpacePlayerController>(GetController());
	if (SpacePlayerController)
	{
		ASpaceHUD* SpaceHUD = Cast<ASpaceHUD>(SpacePlayerController->GetHUD());
		if (SpaceHUD)
		{
			SpaceHUD->OnPlayerSpacecraftInventoryStateChanged();
		}
	}
}

void ASpacePlayerPawn::DestroyWeaponry()
{
	Super::DestroyWeaponry();

	Inventory->ClearAllItems();
}

void ASpacePlayerPawn::Supply_Implementation(AItem* ItemToProvide, EItemTakingAction & ItemTakeAction)
{
	if (ItemToProvide == nullptr)
		return;

	// TODO: maybe this isn't the best way... perhaps use separate entities (interceptors) to handle the receiving?
	if (AAmmunitionPile* AmmoPile = Cast<AAmmunitionPile>(ItemToProvide))
	{
		// Take as much ammo as the spacecraft can carry.

		int32 AmmoAmountNeeded = GetNeededAmmoAmount(AmmoPile->GetAmmoType());
		int32 AmmoAmountFound = AmmoPile->TakeAmmo(AmmoAmountNeeded);
		int32 AmmoAmountAdded;

		SupplyAmmo(AmmoPile->GetAmmoType(), AmmoAmountFound, AmmoAmountAdded);

		ItemTakeAction = 
			AmmoPile->IsEmpty() ? EItemTakingAction::FullyTaken     :
			AmmoAmountAdded > 0 ? EItemTakingAction::PartiallyTaken : EItemTakingAction::None;
	}
	else if (AWeapon* Weapon = Cast<AWeapon>(ItemToProvide))
	{
		// Take the weapon if there's any space left for it aboard the spacecraft.

		bool bHasRoomForNewWeapon = IsSpaceAvailableOnSpacecraft();

		if (bHasRoomForNewWeapon)
		{
			SupplyWeapon(Weapon);
		}

		ItemTakeAction = bHasRoomForNewWeapon ? EItemTakingAction::FullyTaken : EItemTakingAction::None;
	}
}

void ASpacePlayerPawn::CheckCameraOffset(float DeltaTime)
{
	// The speed of the spacecraft will affect the spring arm's length.

	float SpringArmLengthMultiplier    = bIsTurboModeActive ? SpringArmLength_TurboFlightModeMultiplier : SpringArmLength_NormalFlightModeMultiplier;
	float DesiredCameraSpringArmLength = SpringArmLength_SpeedFactor * SpacecraftMovementComponent->Velocity.Size() * SpringArmLengthMultiplier;
	float DesiredCameraFOV             = bIsTurboModeActive ? CameraTurboFlightFOV : CameraNormalFlightFOV;

	if (! FMath::IsNearlyEqual(SpringArmComponent->TargetArmLength, DesiredCameraSpringArmLength))
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, DesiredCameraSpringArmLength, DeltaTime, CameraZoomSpeed);
		CameraComponent->FieldOfView        = FMath::FInterpTo(CameraComponent->FieldOfView, DesiredCameraFOV, DeltaTime, CameraFOVZoomSpeed);
	}
}

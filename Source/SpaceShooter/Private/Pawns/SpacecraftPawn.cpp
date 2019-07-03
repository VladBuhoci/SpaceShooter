// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Pawns/SpacecraftPawn.h"
#include "Components/XYOnlyPhysicsConstraintComponent.h"
#include "Effects/Explosion.h"
#include "Loot/Items/Weapon.h"
#include "Projectiles/Projectile.h"
#include "GameModes/SpaceGameMode.h"

#include "ConstructorHelpers.h"

#include "Engine/World.h"
#include "Engine/StaticMesh.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "Components/StaticMeshComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Camera/CameraComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


/** Sets default values. */
ASpacecraftPawn::ASpacecraftPawn()
{
	PrimaryActorTick.bCanEverTick    = true;

	// Initialize components.
	SpacecraftMeshComponent           = CreateDefaultSubobject<UStaticMeshComponent             >("Spacecraft Mesh Component");
	SpacecraftMovementComponent       = CreateDefaultSubobject<UFloatingPawnMovement            >("Spacecraft Movement Component");
	XYPlanePhysicsConstraintComponent = CreateDefaultSubobject<UXYOnlyPhysicsConstraintComponent>("XY Plane Physics Constraint Component");
	BackSideThrusterParticleEmitter   = CreateDefaultSubobject<UParticleSystemComponent         >("Back Thruster Particle Emitter");
	FrontSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent         >("Front Thruster Particle Emitter");
	LeftSideThrusterParticleEmitter   = CreateDefaultSubobject<UParticleSystemComponent         >("Left Side Thruster Particle Emitter");
	RightSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent         >("Right Side Thruster Particle Emitter");

	RootComponent                     = SpacecraftMeshComponent;

	bIsMovingForward                  = false;
	bIsMovingBackward                 = false;
	bIsTurboModeActive                = false;
	MoveForwardMaxTurboSpeed          = 1400.0f;
	MoveForwardMaxSpeed               = 1000.0f;
	MoveBackwardSpeed                 = 800.0f;
	SpacecraftTurnSpeed               = 10.0f;
	MaxHitPoints                      = 100.0f;
	MaxShieldPoints                   = 200.0f;
	ShieldRechargeRate                = 6.5f;
	ShieldRechargeDelay               = 2.0f;
	bIsShieldRecharging               = false;
	bIsFiringWeapon			          = false;
	Name                              = FText::FromString("Unnamed");
	Faction                           = ESpacecraftFaction::Unspecified;

	// SpacecraftMeshComponent setup:
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpacecraftMeshFinder(TEXT("StaticMesh'/Game/Models/Spacecrafts/Player/SM_PlayerSpacecraft_Dev.SM_PlayerSpacecraft_Dev'"));
	
	if (SpacecraftMeshFinder.Succeeded())
	{
		SpacecraftMeshComponent->SetStaticMesh(SpacecraftMeshFinder.Object);
	}
	// ~ end of SpacecraftMeshComponent setup.

	// SpacecraftMovementComponent setup:
	SpacecraftMovementComponent->UpdatedComponent = SpacecraftMeshComponent;

	// Lock movement on the XY plane.

	// This is required for the next line to work.
	SpacecraftMovementComponent->SetPlaneConstraintEnabled(true);

	// According to the documentation, setting Z to 1.0f prevents the Z value from ever changing,
	//		effectively locking movement on the XY plane.
	SpacecraftMovementComponent->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f));

	// ~ end of SpacecraftMovementComponent setup.

	// XYPlanePhysicsConstraintComponent setup:
	XYPlanePhysicsConstraintComponent->SetupAttachment(RootComponent);
	XYPlanePhysicsConstraintComponent->SetActorConstrainedComponent(RootComponent);
	// ~ end of XYPlanePhysicsConstraintComponent setup.

	// BackSideThrusterParticleEmitter setup:
	BackSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "BackThruster");
	BackSideThrusterParticleEmitter->SetAutoActivate(false);
	BackSideThrusterParticleEmitter->SetTemplate(BacksideNormalThrusterParticleSystem);
	// ~ end of BackSideThrusterParticleEmitter setup.

	// FrontSideThrusterParticleEmitter setup:
	FrontSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "FrontThruster");
	FrontSideThrusterParticleEmitter->SetAutoActivate(false);
	// ~ end of FrontSideThrusterParticleEmitter setup.

	// LeftSideThrusterParticleEmitter setup:
	LeftSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "LeftThruster");
	LeftSideThrusterParticleEmitter->SetAutoActivate(false);
	// ~ end of LeftSideThrusterParticleEmitter setup.

	// RightSideThrusterParticleEmitter setup:
	RightSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "RightThruster");
	RightSideThrusterParticleEmitter->SetAutoActivate(false);
	// ~ end of RightSideThrusterParticleEmitter setup.
}

/** Called when the game starts or when spawned. */
void ASpacecraftPawn::BeginPlay()
{
	Super::BeginPlay();

	AnnounceOwnBirth();
	
	InitializeAttributes();
}

void ASpacecraftPawn::InitializeAttributes()
{
	MoveForwardSpeed    = MoveForwardMaxSpeed;
	CurrentHitPoints    = MaxHitPoints;
	CurrentShieldPoints = MaxShieldPoints;
}

/** Called every frame. */
void ASpacecraftPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Check if we are flying and activate thrusters' particle emitters accordingly.
	if (IsCurrentlyFlying())
	{
		if (bIsMovingForward)
		{
			FrontSideThrusterParticleEmitter->DeactivateSystem();
			
			if (BackSideThrusterParticleEmitter->Template && !BackSideThrusterParticleEmitter->bIsActive)
			{
				BackSideThrusterParticleEmitter->ActivateSystem();
			}
		}
		else if (bIsMovingBackward)
		{
			BackSideThrusterParticleEmitter->DeactivateSystem();

			if (FrontSideThrusterParticleEmitter->Template && !FrontSideThrusterParticleEmitter->bIsActive)
			{
				FrontSideThrusterParticleEmitter->ActivateSystem();
			}
		}
	}
	// If we are not moving, stop thrusters' particle emitters.
	else
	{
		StopMovingSpacecraft();
	}

	CheckShieldStatus(DeltaTime);
	CheckIfWeaponNeedsToBeFired();
}

void ASpacecraftPawn::MaintainForwardMovementSetup()
{
	bIsMovingForward  = true;
	bIsMovingBackward = false;

	SpacecraftMovementComponent->MaxSpeed = MoveForwardSpeed;
}

void ASpacecraftPawn::MaintainBackwardMovementSetup()
{
	bIsMovingBackward = true;
	bIsMovingForward  = false;

	SpacecraftMovementComponent->MaxSpeed = MoveBackwardSpeed;

	if (bIsTurboModeActive)
	{
		DeactivateTurboMode();
	}
}

void ASpacecraftPawn::MoveForward(float Value)
{
	if (Value > 0.0f)
	{
		// Add movement in the correct direction.
		AddMovementInput(GetActorForwardVector(), Value);

		MaintainForwardMovementSetup();
	}
}

void ASpacecraftPawn::MoveBackward(float Value)
{
	if (Value < 0.0f)
	{
		// Add movement in the correct direction.
		AddMovementInput(GetActorForwardVector(), Value);

		MaintainBackwardMovementSetup();
	}
}

UPawnMovementComponent* ASpacecraftPawn::GetMovementComponent() const
{
	return SpacecraftMovementComponent;
}

bool ASpacecraftPawn::IsCurrentlyFlying() const
{
	return SpacecraftMovementComponent->Velocity.Size() > 0.0f;
}

void ASpacecraftPawn::RotateSpacecraft(FRotator Rotator)
{
	FRotator PreviousRotation = SpacecraftMeshComponent->GetComponentRotation();
	FRotator NewRotation      = UKismetMathLibrary::RInterpTo(PreviousRotation, Rotator, FApp::GetDeltaTime(), SpacecraftTurnSpeed);
	
	// If the yaw happens to be negative, calculate the positive value it could have been instead (e.g.: for -40 it is 320).
	PreviousRotation.Yaw = PreviousRotation.Yaw >= 0.0f ? PreviousRotation.Yaw : 360.0f + PreviousRotation.Yaw;
	NewRotation.Yaw      = NewRotation.Yaw      >= 0.0f ? NewRotation.Yaw      : 360.0f + NewRotation.Yaw;

	if (UKismetMathLibrary::Round(PreviousRotation.Yaw) != UKismetMathLibrary::Round(NewRotation.Yaw))
	{
		bool bRotateClockwise = UKismetMathLibrary::Abs(PreviousRotation.Yaw - NewRotation.Yaw) <= 180.0f
			? PreviousRotation.Yaw < NewRotation.Yaw
			: PreviousRotation.Yaw > NewRotation.Yaw;

		if (bRotateClockwise)
		{
			RotateSpacecraftClockwise(NewRotation);
		}
		else
		{
			RotateSpacecraftCounterclockwise(NewRotation);
		}
	}
	else
	{
		StopRotatingSpacecraft();
	}
}

void ASpacecraftPawn::ActivateTurboMode()
{
	// If the spacecraft is currently moving forward, we can enable turbo mode.

	if (bIsMovingForward)
	{
		MoveForwardSpeed = MoveForwardMaxTurboSpeed;
		
		if (BacksideTurboThrusterParticleSystem != NULL)
		{
			BackSideThrusterParticleEmitter->DeactivateSystem();
			BackSideThrusterParticleEmitter->SetTemplate(BacksideTurboThrusterParticleSystem);
			BackSideThrusterParticleEmitter->ActivateSystem();
		}

		bIsTurboModeActive = true;

		OnTurboModeActivated();
	}
}

void ASpacecraftPawn::DeactivateTurboMode()
{
	MoveForwardSpeed = MoveForwardMaxSpeed;

	if (bIsMovingForward)
		BackSideThrusterParticleEmitter->DeactivateSystem();

	if (BacksideNormalThrusterParticleSystem != NULL)
		BackSideThrusterParticleEmitter->SetTemplate(BacksideNormalThrusterParticleSystem);

	if (bIsMovingForward)
		BackSideThrusterParticleEmitter->ActivateSystem();

	bIsTurboModeActive = false;

	OnTurboModeDeactivated();
}


void ASpacecraftPawn::OnTurboModeActivated()
{

}

void ASpacecraftPawn::OnTurboModeDeactivated()
{

}

void ASpacecraftPawn::RotateSpacecraftClockwise(FRotator NewRotation)
{
	RightSideThrusterParticleEmitter->DeactivateSystem();
	
	if (LeftSideThrusterParticleEmitter->Template && !LeftSideThrusterParticleEmitter->bIsActive)
	{
		LeftSideThrusterParticleEmitter->ActivateSystem();
	}

	SpacecraftMeshComponent->SetWorldRotation(NewRotation);
}

void ASpacecraftPawn::RotateSpacecraftCounterclockwise(FRotator NewRotation)
{
	LeftSideThrusterParticleEmitter->DeactivateSystem();

	if (RightSideThrusterParticleEmitter->Template && !RightSideThrusterParticleEmitter->bIsActive)
	{
		RightSideThrusterParticleEmitter->ActivateSystem();
	}

	SpacecraftMeshComponent->SetWorldRotation(NewRotation);
}

void ASpacecraftPawn::StopRotatingSpacecraft()
{
	LeftSideThrusterParticleEmitter->DeactivateSystem();
	RightSideThrusterParticleEmitter->DeactivateSystem();
}

void ASpacecraftPawn::StopMovingSpacecraft()
{
	DeactivateTurboMode();

	bIsMovingForward   = false;
	bIsMovingBackward  = false;

	BackSideThrusterParticleEmitter->DeactivateSystem();
	FrontSideThrusterParticleEmitter->DeactivateSystem();
}

void ASpacecraftPawn::DestroyWeaponry()
{
	DestructWeapon(PreparedWeapons.Slot_1);
	DestructWeapon(PreparedWeapons.Slot_2);
	DestructWeapon(PreparedWeapons.Slot_3);
	DestructWeapon(PreparedWeapons.Slot_4);
}

AWeapon* ASpacecraftPawn::SetWeaponOnPreparedSlot_1(AWeapon* WeaponToAdd, FAttachmentTransformRules & AttachRules,
	bool bEquipNewWeapon)
{
	AWeapon* PreviousWeaponSittingInSlot = PreparedWeapons.Slot_1;

	PreparedWeapons.Slot_1 = WeaponToAdd;
	
	if (PreparedWeapons.Slot_1)
	{
		PreparedWeapons.Slot_1->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));

		if (bEquipNewWeapon)
		{
			EquipWeaponFromSlot_1();
		}
	}

	return PreviousWeaponSittingInSlot;
}

AWeapon* ASpacecraftPawn::SetWeaponOnPreparedSlot_2(AWeapon* WeaponToAdd, FAttachmentTransformRules & AttachRules,
	bool bEquipNewWeapon)
{
	AWeapon* PreviousWeaponSittingInSlot = PreparedWeapons.Slot_2;

	PreparedWeapons.Slot_2 = WeaponToAdd;
	
	if (PreparedWeapons.Slot_2)
	{
		PreparedWeapons.Slot_2->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));

		if (bEquipNewWeapon)
		{
			EquipWeaponFromSlot_2();
		}
	}

	return PreviousWeaponSittingInSlot;
}

AWeapon* ASpacecraftPawn::SetWeaponOnPreparedSlot_3(AWeapon* WeaponToAdd, FAttachmentTransformRules & AttachRules,
	bool bEquipNewWeapon)
{
	AWeapon* PreviousWeaponSittingInSlot = PreparedWeapons.Slot_3;

	PreparedWeapons.Slot_3 = WeaponToAdd;

	if (PreparedWeapons.Slot_3)
	{
		PreparedWeapons.Slot_3->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));

		if (bEquipNewWeapon)
		{
			EquipWeaponFromSlot_3();
		}
	}

	return PreviousWeaponSittingInSlot;
}

AWeapon* ASpacecraftPawn::SetWeaponOnPreparedSlot_4(AWeapon* WeaponToAdd, FAttachmentTransformRules & AttachRules,
	bool bEquipNewWeapon)
{
	AWeapon* PreviousWeaponSittingInSlot = PreparedWeapons.Slot_4;

	PreparedWeapons.Slot_4 = WeaponToAdd;
	
	if (PreparedWeapons.Slot_4)
	{
		PreparedWeapons.Slot_4->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));

		if (bEquipNewWeapon)
		{
			EquipWeaponFromSlot_4();
		}
	}

	return PreviousWeaponSittingInSlot;
}

AWeapon* ASpacecraftPawn::GetWeaponOnPreparedSlot(int32 SlotIndex) const
{
	if (SlotIndex < 1 || SlotIndex > 4)
		return nullptr;

	switch (SlotIndex)
	{
	case 1: return PreparedWeapons.Slot_1;
	case 2: return PreparedWeapons.Slot_2;
	case 3: return PreparedWeapons.Slot_3;
	case 4: return PreparedWeapons.Slot_4;

	default: return nullptr;
	}
}

AWeapon* ASpacecraftPawn::SetWeaponOnPreparedSlot(AWeapon* WeaponToAdd, int32 SlotIndex, bool bEquipNewWeapon)
{
	if (SlotIndex < 1 || SlotIndex > 4)
		return nullptr;

	FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

	switch (SlotIndex)
	{
	case 1: return SetWeaponOnPreparedSlot_1(WeaponToAdd, AttachRules, bEquipNewWeapon);
	case 2: return SetWeaponOnPreparedSlot_2(WeaponToAdd, AttachRules, bEquipNewWeapon);
	case 3: return SetWeaponOnPreparedSlot_3(WeaponToAdd, AttachRules, bEquipNewWeapon);
	case 4: return SetWeaponOnPreparedSlot_4(WeaponToAdd, AttachRules, bEquipNewWeapon);

	default: return nullptr;
	}
}

float ASpacecraftPawn::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// End the previous shield recharge process when hit.
	StopShieldRechargeProcess();
	
	// Damage the shield first.
	// If the shield is down, damage the ship.
	if (CurrentShieldPoints > 0)
	{
		int32 UnabsorbedAbsorbedDamage = Damage - CurrentShieldPoints;	// In case the shield cannot take in all of this damage, keep the remaining bit for the ship.
		int32 ActualAbsorbedDamage     = Damage - FMath::Clamp(UnabsorbedAbsorbedDamage, 0, UnabsorbedAbsorbedDamage);
		int32 DamageTakenByShip        = Damage - ActualAbsorbedDamage;

		CurrentShieldPoints -= ActualAbsorbedDamage;
		CurrentHitPoints    -= DamageTakenByShip;
	}
	else
	{
		CurrentHitPoints -= Damage;
	}
	
	// Schedule a new shield recharge process.
	ScheduleShieldRechargeProcess();

	if (CurrentHitPoints <= 0.0f)
	{
		EndFiringWeapon();

		// Disable shield recharging and put down the shield completely.
		StopShieldRechargeProcess();
		CurrentShieldPoints = 0;

		GetWorldTimerManager().ClearAllTimersForObject(this);

		DestroySpacecraft();
	}

	// Give subclasses a chance to react to this event before returning.
	OnDamageTaken(Cast<ASpacecraftPawn>(DamageCauser));

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

bool ASpacecraftPawn::IsNotDestroyed() const
{
	return CurrentHitPoints > 0.0f;
}

void ASpacecraftPawn::DestroySpacecraft()
{
	// Disable physical interactions so future projectiles overlapping this ship will ignore it.
	SpacecraftMeshComponent->bGenerateOverlapEvents = false;

	// Give subclasses a chance to react before the spacecraft is destroyed.
	bool bShouldPlayDestroyEffects = false;
	bool bShouldBeDestroyedForGood = false;

	PreDestroy(bShouldPlayDestroyEffects, bShouldBeDestroyedForGood);

	if (bShouldPlayDestroyEffects)
	{
		PlayDestroyEffects();
	}

	// Notify SpaceGameMode so it can refresh its array of spacecrafts.
	AnnounceOwnDestruction();

	if (bShouldBeDestroyedForGood)
	{
		// Clean-up.
		DestroyWeaponry();

		// Finally kill this actor.
		Destroy();
	}
}

void ASpacecraftPawn::PlayDestroyEffects()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		if (ExplosionEffectsHandlerClass != NULL)
		{
			AExplosion* DeathExplosion = WorldPtr->SpawnActor<AExplosion>(ExplosionEffectsHandlerClass,
				GetActorLocation(), GetActorRotation());

			if (DeathExplosion)
			{
				DeathExplosion->PlayEffects();
			}
		}
	}
}

void ASpacecraftPawn::AnnounceOwnBirth()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));

		if (SpaceGameMode)
		{
			SpaceGameMode->NotifySpacecraftSpawned(this);
		}
	}
}

void ASpacecraftPawn::AnnounceOwnDestruction()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));

		if (SpaceGameMode)
		{
			SpaceGameMode->NotifySpacecraftDestroyed(this);
		}
	}
}

void ASpacecraftPawn::CheckShieldStatus(float DeltaTime)
{
	if (bIsShieldRecharging)
	{
		RechargeShield(DeltaTime);
	}
}

void ASpacecraftPawn::ScheduleShieldRechargeProcess()
{
	GetWorldTimerManager().SetTimer(ShieldRechargeTimerHandle, this, &ASpacecraftPawn::BeginShieldRechargeProcess, ShieldRechargeDelay, false);
}

void ASpacecraftPawn::BeginShieldRechargeProcess()
{
	bIsShieldRecharging = true;

	GetWorldTimerManager().ClearTimer(ShieldRechargeTimerHandle);
}

void ASpacecraftPawn::StopShieldRechargeProcess()
{
	bIsShieldRecharging = false;

	GetWorldTimerManager().ClearTimer(ShieldRechargeTimerHandle);
}

void ASpacecraftPawn::RechargeShield(float DeltaTime)
{
	CurrentShieldPoints = FMath::FInterpConstantTo(CurrentShieldPoints, MaxShieldPoints, DeltaTime, ShieldRechargeRate);
	
	// If the shield reached full capacity, stop the recharging process.
	if (CurrentShieldPoints == MaxShieldPoints)
	{
		StopShieldRechargeProcess();
	}
}

void ASpacecraftPawn::BeginFiringWeapon()
{
	bIsFiringWeapon = true;
}

void ASpacecraftPawn::EndFiringWeapon()
{
	bIsFiringWeapon = false;
}

void ASpacecraftPawn::EquipWeaponFromSlot_1()
{
	if (PreparedWeapons.Slot_1)
		EquipWeapon(PreparedWeapons.Slot_1);
}

void ASpacecraftPawn::EquipWeaponFromSlot_2()
{
	if (PreparedWeapons.Slot_2)
		EquipWeapon(PreparedWeapons.Slot_2);
}

void ASpacecraftPawn::EquipWeaponFromSlot_3()
{
	if (PreparedWeapons.Slot_3)
		EquipWeapon(PreparedWeapons.Slot_3);
}

void ASpacecraftPawn::EquipWeaponFromSlot_4()
{
	if (PreparedWeapons.Slot_4)
		EquipWeapon(PreparedWeapons.Slot_4);
}

void ASpacecraftPawn::EquipWeaponFromSlot(int32 SlotIndex)
{
	if (SlotIndex < 1 || SlotIndex >= 4)
		return;

	switch (SlotIndex)
	{
	case 1: EquipWeaponFromSlot_1(); break;
	case 2: EquipWeaponFromSlot_2(); break;
	case 3: EquipWeaponFromSlot_3(); break;
	case 4: EquipWeaponFromSlot_4(); break;

	default: break;
	}
}

void ASpacecraftPawn::EquipWeaponFromSlot_FirstValidIndex()
{
	int32 WeaponIndex = GetFirstOccupiedWeaponSlotIndex();
	
	EquipWeaponFromSlot(WeaponIndex);
}

void ASpacecraftPawn::EquipWeaponFromSlot_Random()
{
	int32 WeaponIndex = GetRandomOccupiedWeaponSlotIndex();

	EquipWeaponFromSlot(WeaponIndex);
}

void ASpacecraftPawn::RemoveWeaponFromSlot(int32 SlotIndex)
{
	if (SlotIndex < 1 || SlotIndex > 4)
		return;

	// Check if the weapon to be removed is the one we're currently using.

	bool bWeaponToRemoveIsEquipped = EquippedWeapon == GetWeaponOnPreparedSlot(SlotIndex);
	AWeapon* RemovedWeapon = nullptr;
	
	switch (SlotIndex)
	{
	case 1: RemovedWeapon = PreparedWeapons.Slot_1; PreparedWeapons.Slot_1 = nullptr; break;
	case 2: RemovedWeapon = PreparedWeapons.Slot_2; PreparedWeapons.Slot_2 = nullptr; break;
	case 3: RemovedWeapon = PreparedWeapons.Slot_3; PreparedWeapons.Slot_3 = nullptr; break;
	case 4: RemovedWeapon = PreparedWeapons.Slot_4; PreparedWeapons.Slot_4 = nullptr; break;
	}
	
	if (bWeaponToRemoveIsEquipped)
	{
		UnequipCurrentWeapon();

		AWeapon* WeaponToReplaceRemovedOne = GetWeaponOnPreparedSlot(GetFirstOccupiedWeaponSlotIndex());

		EquipWeapon(WeaponToReplaceRemovedOne);
	}

	if (RemovedWeapon)
		RemovedWeapon->Destroy();

	OnActiveWeaponSlotsStateChanged();
}

bool ASpacecraftPawn::IsSpaceAvailableForAnotherWeapon()
{
	return PreparedWeapons.Slot_1 == nullptr
		|| PreparedWeapons.Slot_2 == nullptr
		|| PreparedWeapons.Slot_3 == nullptr
		|| PreparedWeapons.Slot_4 == nullptr;
}

int32 ASpacecraftPawn::GetFirstFreeWeaponSlotIndex()
{
	return
		PreparedWeapons.Slot_1 == nullptr ? 1 :
		PreparedWeapons.Slot_2 == nullptr ? 2 :
		PreparedWeapons.Slot_3 == nullptr ? 3 :
		PreparedWeapons.Slot_4 == nullptr ? 4 : 0;
}

int32 ASpacecraftPawn::GetFirstOccupiedWeaponSlotIndex()
{
	return
		PreparedWeapons.Slot_1 != nullptr ? 1 :
		PreparedWeapons.Slot_2 != nullptr ? 2 :
		PreparedWeapons.Slot_3 != nullptr ? 3 :
		PreparedWeapons.Slot_4 != nullptr ? 4 : 0;
}

int32 ASpacecraftPawn::GetRandomOccupiedWeaponSlotIndex()
{
	if (!HasAnyWeapons())
		return 0;

	TArray<int32> SlotIndices;

	if (PreparedWeapons.Slot_1)	SlotIndices.Add(1);
	if (PreparedWeapons.Slot_2)	SlotIndices.Add(2);
	if (PreparedWeapons.Slot_3)	SlotIndices.Add(3);
	if (PreparedWeapons.Slot_4)	SlotIndices.Add(4);

	int32 RandIndex = FMath::RandRange(0, SlotIndices.Num() - 1);

	return SlotIndices[RandIndex];
}

void ASpacecraftPawn::FireWeapon_Internal()
{
	if (EquippedWeapon && HasAmmoStockForWeaponType(EquippedWeapon->GetType()))
	{
		EquippedWeapon->FireWeapon(this, AmmoPools[EquippedWeapon->GetType()]);
	}
}

void ASpacecraftPawn::CheckIfWeaponNeedsToBeFired()
{
	if (bIsFiringWeapon && !bIsTurboModeActive)
	{
		FireWeapon_Internal();
	}
}

void ASpacecraftPawn::DestructWeapon(AWeapon* WeaponToDestroy)
{
	if (WeaponToDestroy)
	{
		WeaponToDestroy->Destroy();
	}
}

// TODO: slow down the process, add some visual effect?
void ASpacecraftPawn::EquipWeapon(AWeapon* WeaponToEquip)
{
	UnequipCurrentWeapon();

	if (WeaponToEquip != nullptr)
	{
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetVisibility(true);

		OnActiveWeaponSlotsStateChanged();
	}
}

// TODO: slow down the process, add some visual effect?
void ASpacecraftPawn::UnequipCurrentWeapon()
{
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->SetVisibility(false);
		EquippedWeapon = nullptr;
	}
}

int32 ASpacecraftPawn::GetMaximumAmmoCapacity(EWeaponType WeaponTypeAmmo) const
{
	return AmmoPools[WeaponTypeAmmo].MaxAmmoQuantity;
}

int32 ASpacecraftPawn::GetRemainingAmmo(EWeaponType WeaponTypeAmmo) const
{
	return AmmoPools[WeaponTypeAmmo].CurrentAmmoQuantity;
}

int32 ASpacecraftPawn::GetNeededAmmoAmount(EWeaponType WeaponTypeAmmo) const
{
	return GetMaximumAmmoCapacity(WeaponTypeAmmo) - GetRemainingAmmo(WeaponTypeAmmo);
}

bool ASpacecraftPawn::HasAmmoStockForWeaponType(EWeaponType Type) const
{
	return AmmoPools.Contains(Type);
}

void ASpacecraftPawn::SupplyAmmo(EWeaponType WeaponTypeAmmo, int32 AmmoAmountToAdd, int32 & AmmoAdded)
{
	int32 ActualAmmoAmount = FMath::Clamp(AmmoAmountToAdd, 0, AmmoPools[WeaponTypeAmmo].MaxAmmoQuantity - AmmoPools[WeaponTypeAmmo].CurrentAmmoQuantity);

	AmmoPools[WeaponTypeAmmo].CurrentAmmoQuantity += ActualAmmoAmount;

	AmmoAdded = ActualAmmoAmount;
}

void ASpacecraftPawn::SupplyWeapon(AWeapon* NewWeapon)
{
	int32 FreeSlotIndex = GetFirstFreeWeaponSlotIndex();

	if (FreeSlotIndex > 0)
	{
		SetWeaponOnPreparedSlot(NewWeapon, FreeSlotIndex);
	}
	else
	{
		AddItemToInventory(NewWeapon);
	}
}

TArray<TSubclassOf<UWeaponBlueprint>> ASpacecraftPawn::GetStartingWeaponBlueprintTypes() const
{
	TArray<TSubclassOf<UWeaponBlueprint>> BPTypes;

	if (StartingWeapons.BP_1) BPTypes.Add(StartingWeapons.BP_1);
	if (StartingWeapons.BP_2) BPTypes.Add(StartingWeapons.BP_2);
	if (StartingWeapons.BP_3) BPTypes.Add(StartingWeapons.BP_3);
	if (StartingWeapons.BP_4) BPTypes.Add(StartingWeapons.BP_4);

	return BPTypes;
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacecraftPawn.h"
#include "SpacecraftExplosion.h"
#include "Weapon.h"
#include "Projectile.h"

#include "ConstructorHelpers.h"

#include "Engine/World.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "Components/StaticMeshComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Camera/CameraComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "Runtime/Engine/Public/TimerManager.h"


/** Sets default values. */
ASpacecraftPawn::ASpacecraftPawn()
{
	PrimaryActorTick.bCanEverTick    = true;

	// Initialize components.
	SpacecraftMeshComponent          = CreateDefaultSubobject<UStaticMeshComponent    >("Spacecraft Mesh Component");
	SpacecraftMovementComponent      = CreateDefaultSubobject<UFloatingPawnMovement   >("Spacecraft Movement Component");
	BackSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent>("Back Thruster Particle Emitter");
	FrontSideThrusterParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>("Front Thruster Particle Emitter");
	LeftSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent>("Left Side Thruster Particle Emitter");
	RightSideThrusterParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>("Right Side Thruster Particle Emitter");

	RootComponent                    = SpacecraftMeshComponent;

	bIsMovingForward                 = false;
	bIsMovingBackward                = false;
	bIsTurboModeActive               = false;
	MoveForwardMaxTurboSpeed         = 1400.0f;
	MoveForwardMaxSpeed              = 1000.0f;
	MoveBackwardSpeed                = 800.0f;
	SpacecraftTurnSpeed              = 10.0f;
	MaxHitPoints                     = 100.0f;
	MaxShieldPoints                  = 200.0f;
	ShieldRechargeRate               = 6.5f;
	ShieldRechargeDelay              = 2.0f;
	bIsShieldRecharging              = false;
	bIsFiringWeapon			         = false;
	Faction                          = ESpacecraftFaction::Unspecified;

	// SpacecraftMeshComponent setup:
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpacecraftMeshFinder(TEXT("StaticMesh'/Game/Models/Spacecrafts/Player/SM_PlayerSpacecraft_Dev.SM_PlayerSpacecraft_Dev'"));
	
	if (SpacecraftMeshFinder.Succeeded())
	{
		SpacecraftMeshComponent->SetStaticMesh(SpacecraftMeshFinder.Object);
	}
	// ~ end of SpacecraftMeshComponent setup.

	// FloatingPawnMovementComponent setup:
	SpacecraftMovementComponent->UpdatedComponent = SpacecraftMeshComponent;
	// ~ end of FloatingPawnMovementComponent setup.

	// BackSideThrusterParticleEmitter setup:
	BackSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "BackThruster");
	BackSideThrusterParticleEmitter->SetTemplate(BacksideNormalThrusterParticleSystem);
	// ~ end of BackSideThrusterParticleEmitter setup.

	// FrontSideThrusterParticleEmitter setup:
	FrontSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "FrontThruster");
	// ~ end of FrontSideThrusterParticleEmitter setup.

	// LeftSideThrusterParticleEmitter setup:
	LeftSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "LeftThruster");
	// ~ end of LeftSideThrusterParticleEmitter setup.

	// RightSideThrusterParticleEmitter setup:
	RightSideThrusterParticleEmitter->SetupAttachment(SpacecraftMeshComponent, "RightThruster");
	// ~ end of RightSideThrusterParticleEmitter setup.
}

/** Called when the game starts or when spawned. */
void ASpacecraftPawn::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeAttributes();
	InitializeWeaponry();

	EquipWeaponFromSlot_1();
}

void ASpacecraftPawn::InitializeAttributes()
{
	MoveForwardSpeed    = MoveForwardMaxSpeed;
	CurrentHitPoints    = MaxHitPoints;
	CurrentShieldPoints = MaxShieldPoints;

	AmmoPools.Add(EWeaponType::Blaster, FAmmunitionStock(128));
	AmmoPools.Add(EWeaponType::Cannon, FAmmunitionStock(256));
	AmmoPools.Add(EWeaponType::Shotgun, FAmmunitionStock(64));
	AmmoPools.Add(EWeaponType::Launcher, FAmmunitionStock(32));
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
	FRotator previousRotation = SpacecraftMeshComponent->GetComponentRotation();
	FRotator newRotation      = UKismetMathLibrary::RInterpTo(previousRotation, Rotator, FApp::GetDeltaTime(), SpacecraftTurnSpeed);
	
	// If the yaw happens to be negative, calculate the positive value it could have been instead (e.g.: for -40 it is 320).
	previousRotation.Yaw = previousRotation.Yaw >= 0.0f ? previousRotation.Yaw : 360.0f + previousRotation.Yaw;
	newRotation.Yaw      = newRotation.Yaw      >= 0.0f ? newRotation.Yaw      : 360.0f + newRotation.Yaw;

	if (UKismetMathLibrary::Round(previousRotation.Yaw) != UKismetMathLibrary::Round(newRotation.Yaw))
	{
		bool bRotateClockwise = UKismetMathLibrary::Abs(previousRotation.Yaw - newRotation.Yaw) <= 180.0f ? previousRotation.Yaw < newRotation.Yaw : previousRotation.Yaw > newRotation.Yaw;

		if (bRotateClockwise)
		{
			RotateSpacecraftClockwise(newRotation);
		}
		else
		{
			RotateSpacecraftCounterclockwise(newRotation);
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

void ASpacecraftPawn::InitializeWeaponry()
{
	if (WeaponClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			PreparedWeapons.Slot_1 = ConstructWeapon(World);
			PreparedWeapons.Slot_2 = ConstructWeapon(World);
			PreparedWeapons.Slot_3 = ConstructWeapon(World);
			PreparedWeapons.Slot_4 = ConstructWeapon(World);
		}
	}
}

void ASpacecraftPawn::DestroyWeaponry()
{
	PreparedWeapons.Slot_1->Destroy();
	PreparedWeapons.Slot_2->Destroy();
	PreparedWeapons.Slot_3->Destroy();
	PreparedWeapons.Slot_4->Destroy();
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
		// TODO: later on, this should be changed to something else to support more weapons.
		EndFiringWeapon();

		// Disable shield recharging and put down the shield completely.
		StopShieldRechargeProcess();
		CurrentShieldPoints = 0;

		DestroySpacecraft();
	}

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
			WorldPtr->SpawnActor<ASpacecraftExplosion>(ExplosionEffectsHandlerClass, GetActorLocation(), GetActorRotation());
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
	EquipWeapon(PreparedWeapons.Slot_1);
}

void ASpacecraftPawn::EquipWeaponFromSlot_2()
{
	EquipWeapon(PreparedWeapons.Slot_2);
}

void ASpacecraftPawn::EquipWeaponFromSlot_3()
{
	EquipWeapon(PreparedWeapons.Slot_3);
}

void ASpacecraftPawn::EquipWeaponFromSlot_4()
{
	EquipWeapon(PreparedWeapons.Slot_4);
}

void ASpacecraftPawn::FireWeapon_Internal()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->FireWeapon(this, AmmoPools[EquippedWeapon->GetType()].CurrentAmmoQuantity);
	}
}

void ASpacecraftPawn::CheckIfWeaponNeedsToBeFired()
{
	if (bIsFiringWeapon && !bIsTurboModeActive)
	{
		FireWeapon_Internal();
	}
}

AWeapon* ASpacecraftPawn::ConstructWeapon(UWorld* World)
{
	AWeapon* NewWeapon = World->SpawnActor<AWeapon>(WeaponClass);
	
	if (NewWeapon)
	{
		FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

		NewWeapon->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));
	}

	return NewWeapon;
}

// TODO: slow down the process, add some visual effect?
void ASpacecraftPawn::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip != nullptr)
	{
		UnequipCurrentWeapon();

		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetVisibility(true);
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

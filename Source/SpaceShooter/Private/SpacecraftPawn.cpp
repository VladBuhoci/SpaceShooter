// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacecraftPawn.h"
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
#include "Kismet/GameplayStatics.h"


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
	MoveForwardSpeed                 = MoveForwardMaxSpeed;
	MoveBackwardSpeed                = 800.0f;
	SpacecraftTurnSpeed              = 10.0f;
	MaxHitPoints                     = 100.0f;
	CurrentHitPoints                 = MaxHitPoints;
	bIsFiringPrimaryWeapons			 = false;
	Faction                          = ESpacecraftFaction::Unspecified;

	// SpacecraftMeshComponent setup:
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpacecraftMeshFinder(TEXT("StaticMesh'/Game/StaticMeshes/Spacecrafts/Player/PlayerSpacecraft_Dev.PlayerSpacecraft_Dev'"));
	
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
	
	InitializeWeaponry();
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

	CheckIfWeaponsNeedToBeFired();
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

void ASpacecraftPawn::RotateSpacecraft(FRotator rotator)
{
	FRotator previousRotation = SpacecraftMeshComponent->GetComponentRotation();
	FRotator newRotation      = UKismetMathLibrary::RInterpTo(previousRotation, rotator, FApp::GetDeltaTime(), SpacecraftTurnSpeed);
	
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

void ASpacecraftPawn::RotateSpacecraftClockwise(FRotator newRotation)
{
	RightSideThrusterParticleEmitter->DeactivateSystem();
	
	if (LeftSideThrusterParticleEmitter->Template && !LeftSideThrusterParticleEmitter->bIsActive)
	{
		LeftSideThrusterParticleEmitter->ActivateSystem();
	}

	SpacecraftMeshComponent->SetWorldRotation(newRotation);
}

void ASpacecraftPawn::RotateSpacecraftCounterclockwise(FRotator newRotation)
{
	LeftSideThrusterParticleEmitter->DeactivateSystem();

	if (RightSideThrusterParticleEmitter->Template && !RightSideThrusterParticleEmitter->bIsActive)
	{
		RightSideThrusterParticleEmitter->ActivateSystem();
	}

	SpacecraftMeshComponent->SetWorldRotation(newRotation);
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
	if (PrimaryWeaponClass)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			PrimaryWeapon = World->SpawnActor<AWeapon>(PrimaryWeaponClass);

			if (PrimaryWeapon)
			{
				FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);

				PrimaryWeapon->AttachToComponent(SpacecraftMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));
			}
		}
	}
}

void ASpacecraftPawn::DestroyWeaponry()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->Destroy();
	}
}

// TODO: W.I.P.
float ASpacecraftPawn::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHitPoints -= Damage;

	if (CurrentHitPoints <= 0.0f)
		DestroySpacecraft();

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

bool ASpacecraftPawn::IsNotDestroyed() const
{
	return CurrentHitPoints > 0.0f;
}

void ASpacecraftPawn::DestroySpacecraft()
{
	PlayDestroyEffects();

	// Disable physical interactions so future projectiles overlapping this ship will ignore it.
	SpacecraftMeshComponent->bGenerateOverlapEvents = false;

	// Clean-up.
	DestroyWeaponry();

	// Finally kill this actor.
	Destroy();
}

void ASpacecraftPawn::PlayDestroyEffects()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		// Play destruction sound.
		if (DestroySound)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldPtr, DestroySound, this->GetActorLocation());
		}

		// Spawn the destruction effect particles.
		if (DestroyParticleEffect != NULL)
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldPtr, DestroyParticleEffect, this->GetActorLocation());
		}
	}
}

void ASpacecraftPawn::BeginFiringPrimaryWeapons()
{
	bIsFiringPrimaryWeapons = true;
}

void ASpacecraftPawn::EndFiringPrimaryWeapons()
{
	bIsFiringPrimaryWeapons = false;
}

void ASpacecraftPawn::FirePrimaryWeapons_Internal()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->FireWeapon(this);
	}
}

void ASpacecraftPawn::CheckIfWeaponsNeedToBeFired()
{
	if (bIsFiringPrimaryWeapons && !bIsTurboModeActive)
	{
		FirePrimaryWeapons_Internal();
	}
}
// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacePlayerPawn.h"
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


/** Sets default values. */
ASpacePlayerPawn::ASpacePlayerPawn()
{
	PrimaryActorTick.bCanEverTick    = true;

	// Initialize components.
	CentralPlayerSceneComponent      = CreateDefaultSubobject<USceneComponent         >("Central Player Scene Component");
	SpaceshipMeshComponent           = CreateDefaultSubobject<UStaticMeshComponent    >("Spaceship Mesh Component");
	SpaceshipMovementComponent       = CreateDefaultSubobject<UFloatingPawnMovement   >("Spaceship Movement Component");
	CameraComponent                  = CreateDefaultSubobject<UCameraComponent        >("Camera Component");
	SpringArmComponent               = CreateDefaultSubobject<USpringArmComponent     >("Spring Arm Component");
	BackSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent>("Back Thruster Particle Emitter");
	FrontSideThrusterParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>("Front Thruster Particle Emitter");
	LeftSideThrusterParticleEmitter  = CreateDefaultSubobject<UParticleSystemComponent>("Left Side Thruster Particle Emitter");
	RightSideThrusterParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>("Right Side Thruster Particle Emitter");

	RootComponent                    = SpaceshipMeshComponent;

	bIsMovingForward                 = false;
	bIsMovingBackward                = false;
	bIsTurboModeActive               = false;
	MoveForwardMaxTurboSpeed         = 1600.0f;
	MoveForwardMaxSpeed              = 1200.0f;
	MoveForwardSpeed                 = MoveForwardMaxSpeed;
	MoveBackwardSpeed                = 900.0f;
	SpringArmOffset                  = FVector(-500.0f, 0.0f, 600.0f);
	SpringArmRotation                = FRotator(-50.0f, 0.0f, 0.0f);
	SpringArmTurboLength             = 100.0f;
	SpaceshipTurnSpeed               = 10.0f;
	bIsFiringPrimaryWeapons			 = false;

	// CentralPlayerSceneComponent setup:
	CentralPlayerSceneComponent->SetupAttachment(SpaceshipMeshComponent);
	// ~ end of CentralPlayerSceneComponent setup.

	// SpaceshipMeshComponent setup:
	ConstructorHelpers::FObjectFinder<UStaticMesh> spaceshipMeshFinder (TEXT("StaticMesh'/Game/StaticMeshes/Spaceships/PlayerSpaceship/PlayerSpaceship_Dev.PlayerSpaceship_Dev'"));
	
	if (spaceshipMeshFinder.Succeeded())
	{
		SpaceshipMeshComponent->SetStaticMesh(spaceshipMeshFinder.Object);
	}
	// ~ end of SpaceshipMeshComponent setup.

	// SpringArmComponent setup:
	SpringArmComponent->SetupAttachment(CentralPlayerSceneComponent);
	SpringArmComponent->SetRelativeLocation(SpringArmOffset);
	SpringArmComponent->SetRelativeRotation(SpringArmRotation);
	SpringArmComponent->TargetArmLength          = 0.0f;
	SpringArmComponent->bEnableCameraLag         = true;						// Enable spring arm lag.
	SpringArmComponent->bEnableCameraRotationLag = false;						// Disable spring arm rotation lag. (camera never rotates)
	SpringArmComponent->CameraLagSpeed           = 20.0f;
	// ~ end of SpringArmComponent setup.

	// CameraComponent setup:
	CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);
	CameraComponent->bUsePawnControlRotation = false;					// Do not rotate the camera along with the player.
	// ~ end of CameraComponent setup.

	// FloatingPawnMovementComponent setup:
	SpaceshipMovementComponent->UpdatedComponent = SpaceshipMeshComponent;
	// ~ end of FloatingPawnMovementComponent setup.

	// BackSideThrusterParticleEmitter setup:
	BackSideThrusterParticleEmitter->SetupAttachment(SpaceshipMeshComponent, "BackThruster");
	BackSideThrusterParticleEmitter->SetTemplate(BacksideNormalThrusterParticleSystem);
	// ~ end of BackSideThrusterParticleEmitter setup.

	// FrontSideThrusterParticleEmitter setup:
	FrontSideThrusterParticleEmitter->SetupAttachment(SpaceshipMeshComponent, "FrontThruster");
	//FrontSideThrusterParticleEmitter->SetTemplate();
	// ~ end of FrontSideThrusterParticleEmitter setup.

	// LeftSideThrusterParticleEmitter setup:
	LeftSideThrusterParticleEmitter->SetupAttachment(SpaceshipMeshComponent, "LeftThruster");
	//LeftSideThrusterParticleEmitter->SetTemplate();
	// ~ end of LeftSideThrusterParticleEmitter setup.

	// RightSideThrusterParticleEmitter setup:
	RightSideThrusterParticleEmitter->SetupAttachment(SpaceshipMeshComponent, "RightThruster");
	//RightSideThrusterParticleEmitter->SetTemplate();
	// ~ end of RightSideThrusterParticleEmitter setup.
}

/** Called when the game starts or when spawned. */
void ASpacePlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeWeaponry();
}

/** Called every frame. */
void ASpacePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Keep the helper scene component's rotation at a fixed zero value on all axis so the
	//		camera's spring arm (that is attached to it) never rotates its children (i.e. the camera).
	CentralPlayerSceneComponent->SetWorldRotation(FRotator::ZeroRotator);

	// Check if we are moving and activate thrusters' particle emitters accordingly.
	if (SpaceshipMovementComponent->Velocity.Size() > 0.0f)
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
		StopMovingSpaceship();
	}

	CheckIfWeaponsNeedToBeFired();
}

void ASpacePlayerPawn::MoveForward(float Value)
{
	if (Value > 0.0f)
	{
		// Add movement in the correct direction.
		AddMovementInput(GetActorForwardVector(), Value);

		bIsMovingForward = true;
		bIsMovingBackward = false;

		SpaceshipMovementComponent->MaxSpeed = MoveForwardSpeed;
	}
}

void ASpacePlayerPawn::MoveBackward(float Value)
{
	if (Value < 0.0f)
	{
		// Add movement in the correct direction.
		AddMovementInput(GetActorForwardVector(), Value);

		bIsMovingBackward = true;
		bIsMovingForward = false;

		SpaceshipMovementComponent->MaxSpeed = MoveBackwardSpeed;

		if (bIsTurboModeActive)
			DeactivateTurboMode();
	}
}

UPawnMovementComponent * ASpacePlayerPawn::GetMovementComponent() const
{
	return SpaceshipMovementComponent;
}

void ASpacePlayerPawn::RotateSpaceship(FRotator rotator)
{
	FRotator previousRotation = SpaceshipMeshComponent->GetComponentRotation();
	FRotator newRotation      = UKismetMathLibrary::RInterpTo(previousRotation, rotator, FApp::GetDeltaTime(), SpaceshipTurnSpeed);
	
	// If the yaw happens to be negative, calculate the positive value it could have been instead (e.g.: for -40 it is 320).
	previousRotation.Yaw = previousRotation.Yaw >= 0.0f ? previousRotation.Yaw : 360.0f + previousRotation.Yaw;
	newRotation.Yaw      = newRotation.Yaw      >= 0.0f ? newRotation.Yaw      : 360.0f + newRotation.Yaw;

	if (UKismetMathLibrary::Round(previousRotation.Yaw) != UKismetMathLibrary::Round(newRotation.Yaw))
	{
		bool bRotateClockwise = UKismetMathLibrary::Abs(previousRotation.Yaw - newRotation.Yaw) <= 180.0f ? previousRotation.Yaw < newRotation.Yaw : previousRotation.Yaw > newRotation.Yaw;

		if (bRotateClockwise)
		{
			RotateSpaceshipClockwise(newRotation);
		}
		else
		{
			RotateSpaceshipCounterclockwise(newRotation);
		}
	}
	else
	{
		StopRotatingSpaceship();
	}
}

void ASpacePlayerPawn::ActivateTurboMode()
{
	// If the player is currently moving forward, we can enable turbo mode.

	if (bIsMovingForward)
	{
		MoveForwardSpeed = MoveForwardMaxTurboSpeed;
		
		if (BacksideTurboThrusterParticleSystem != NULL)
		{
			BackSideThrusterParticleEmitter->DeactivateSystem();
			BackSideThrusterParticleEmitter->SetTemplate(BacksideTurboThrusterParticleSystem);
			BackSideThrusterParticleEmitter->ActivateSystem();
		}

		SpringArmComponent->TargetArmLength = SpringArmTurboLength;

		bIsTurboModeActive = true;
		
		// Do not allow any kind of weapon to be used while in turbo mode.
		EndFiringPrimaryWeapons();
	}
}

void ASpacePlayerPawn::DeactivateTurboMode()
{
	MoveForwardSpeed = MoveForwardMaxSpeed;

	if (bIsMovingForward)
		BackSideThrusterParticleEmitter->DeactivateSystem();

	if (BacksideNormalThrusterParticleSystem != NULL)
		BackSideThrusterParticleEmitter->SetTemplate(BacksideNormalThrusterParticleSystem);

	if (bIsMovingForward)
		BackSideThrusterParticleEmitter->ActivateSystem();

	SpringArmComponent->TargetArmLength = 0.0f;

	bIsTurboModeActive = false;
}

void ASpacePlayerPawn::RotateSpaceshipClockwise(FRotator newRotation)
{
	RightSideThrusterParticleEmitter->DeactivateSystem();
	
	if (LeftSideThrusterParticleEmitter->Template && !LeftSideThrusterParticleEmitter->bIsActive)
	{
		LeftSideThrusterParticleEmitter->ActivateSystem();
	}

	SpaceshipMeshComponent->SetWorldRotation(newRotation);
}

void ASpacePlayerPawn::RotateSpaceshipCounterclockwise(FRotator newRotation)
{
	LeftSideThrusterParticleEmitter->DeactivateSystem();

	if (RightSideThrusterParticleEmitter->Template && !RightSideThrusterParticleEmitter->bIsActive)
	{
		RightSideThrusterParticleEmitter->ActivateSystem();
	}

	SpaceshipMeshComponent->SetWorldRotation(newRotation);
}

void ASpacePlayerPawn::StopRotatingSpaceship()
{
	LeftSideThrusterParticleEmitter->DeactivateSystem();
	RightSideThrusterParticleEmitter->DeactivateSystem();
}

void ASpacePlayerPawn::StopMovingSpaceship()
{
	DeactivateTurboMode();

	bIsMovingForward   = false;
	bIsMovingBackward  = false;

	BackSideThrusterParticleEmitter->DeactivateSystem();
	FrontSideThrusterParticleEmitter->DeactivateSystem();
}

void ASpacePlayerPawn::InitializeWeaponry()
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

				PrimaryWeapon->AttachToComponent(SpaceshipMeshComponent, AttachRules, TEXT("Weapon_AttachPoint_DEV"));
			}
		}
	}
}

void ASpacePlayerPawn::BeginFiringPrimaryWeapons()
{
	// Only allow any kind of weapon to be used while not in turbo mode.
	if (!bIsTurboModeActive)
	{
		bIsFiringPrimaryWeapons = true;
	}
}

void ASpacePlayerPawn::EndFiringPrimaryWeapons()
{
	bIsFiringPrimaryWeapons = false;
}

void ASpacePlayerPawn::FirePrimaryWeapons_Internal()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->FireWeapon(EProjectileOwnerType::Friendly);
	}
}

void ASpacePlayerPawn::CheckIfWeaponsNeedToBeFired()
{
	if (bIsFiringPrimaryWeapons)
	{
		FirePrimaryWeapons_Internal();
	}
}
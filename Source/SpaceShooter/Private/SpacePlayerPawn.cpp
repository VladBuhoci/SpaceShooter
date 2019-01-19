// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacePlayerPawn.h"
#include "Weapon.h"
#include "Projectile.h"
#include "SpacecraftTurboModeCameraShake.h"
#include "SpacecraftDestructionCameraShake.h"
#include "SpacePlayerController.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Controller.h"

#include "Components/StaticMeshComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Camera/CameraComponent.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"


/** Sets default values. */
ASpacePlayerPawn::ASpacePlayerPawn()
{
	// Initialize components.
	CentralSceneComponent        = CreateDefaultSubobject<USceneComponent    >("Central Player Scene Component");
	CameraComponent              = CreateDefaultSubobject<UCameraComponent   >("Camera Component");
	SpringArmComponent           = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	
	MoveForwardMaxTurboSpeed     = 2000.0f;
	MoveForwardMaxSpeed          = 1200.0f;
	MoveBackwardSpeed            = 900.0f;
	MaxHitPoints                 = 100.0f;
	MaxShieldPoints              = 200.0f;
	ShieldAbsorptionRate         = 30.0f;
	ShieldRechargeRate           = 4.0f;
	ShieldRechargeDelay          = 2.0f;
	SpringArmOffset              = FVector(-600.0f, 0.0f, 700.0f);
	SpringArmRotation            = FRotator(-50.0f, 0.0f, 0.0f);
	SpacecraftTurnSpeed          = 10.0f;
	DesiredCameraSpringArmLength = 0.0f;
	CameraZoomSpeed              = 10.0f;
	Faction                      = ESpacecraftFaction::Human;

	// CentralSceneComponent setup:
	CentralSceneComponent->SetupAttachment(SpacecraftMeshComponent);
	// ~ end of CentralSceneComponent setup.

	// SpringArmComponent setup:
	SpringArmComponent->SetupAttachment(CentralSceneComponent);
	SpringArmComponent->SetRelativeLocation(SpringArmOffset);
	SpringArmComponent->SetRelativeRotation(SpringArmRotation);
	SpringArmComponent->TargetArmLength          = 0.0f;
	SpringArmComponent->bEnableCameraLag         = true;					// Enable spring arm lag.
	SpringArmComponent->bEnableCameraRotationLag = false;					// Disable spring arm rotation lag. (camera never rotates)
	SpringArmComponent->CameraLagSpeed           = 20.0f;
	// ~ end of SpringArmComponent setup.

	// CameraComponent setup:
	CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);
	CameraComponent->bUsePawnControlRotation     = false;					// Do not rotate the camera along with the player.
	// ~ end of CameraComponent setup.
}

/** Called when the game starts or when spawned. */
void ASpacePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

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

// TODO: W.I.P.
void ASpacePlayerPawn::DestroySpacecraft()
{
	UWorld* World = GetWorld();
	if (World)
	{
		// Play camera shake effect.
		UGameplayStatics::PlayWorldCameraShake(World, USpacecraftDestructionCameraShake::StaticClass(), GetActorLocation(), 0.0f, 10000.0f, 0.0f);

		// Simulate slow motion.
 		UGameplayStatics::SetGlobalTimeDilation(World, 0.25f);
 		UGameplayStatics::SetGlobalPitchModulation(World, 0.75f, 1.0f);

		// Calling these implies scheduling a method call to reset these global values after a bit of time.
		FTimerHandle DisableSlowmotionTimerHandle;

		GetWorldTimerManager().SetTimer(DisableSlowmotionTimerHandle, [World]() {
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			UGameplayStatics::SetGlobalPitchModulation(World, 1.0f, 1.0f);
		}, 0.75f, false);
	}

	// Disable physical interactions so future projectiles overlapping this ship will ignore it.
	SpacecraftMeshComponent->bGenerateOverlapEvents = false;

	// Make the player's ship invisible.
	SpacecraftMeshComponent->SetVisibility(false, true);

	if (ASpacePlayerController* MyController = Cast<ASpacePlayerController>(GetController()))
	{
		MyController->SignalPlayerDied();
	}
}

void ASpacePlayerPawn::BeginFiringPrimaryWeapons()
{
	Super::BeginFiringPrimaryWeapons();

}

void ASpacePlayerPawn::EndFiringPrimaryWeapons()
{
	Super::EndFiringPrimaryWeapons();

}

void ASpacePlayerPawn::CheckCameraOffset(float DeltaTime)
{
	// The speed of the spacecraft will affect the spring arm's length.

	float speedPercentage            = 0.2f;	// 20% of the spacecraft's velocity is used as base value for the spring arm's length.
	float normalFlightModeMultiplier = 1.0f;
	float turboFlightModeMultiplier  = 1.5f;

	DesiredCameraSpringArmLength = speedPercentage * SpacecraftMovementComponent->Velocity.Size() * (bIsTurboModeActive ? turboFlightModeMultiplier : normalFlightModeMultiplier);

	if (! FMath::IsNearlyEqual(SpringArmComponent->TargetArmLength, DesiredCameraSpringArmLength))
	{
		SpringArmComponent->TargetArmLength = FMath::FInterpTo(SpringArmComponent->TargetArmLength, DesiredCameraSpringArmLength, DeltaTime, CameraZoomSpeed);
	}
}

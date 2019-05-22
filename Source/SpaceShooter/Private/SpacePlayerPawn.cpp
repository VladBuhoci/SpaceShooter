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
	ShieldRechargeRate           = 10.0f;
	ShieldRechargeDelay          = 1.25f;
	SpacecraftTurnSpeed          = 10.0f;
	SpringArmLength_SpeedFactor  = 0.25f;	// 25% of the spacecraft's velocity is used as base value for the spring arm's length.
	SpringArmLength_NormalFlightModeMultiplier = 1.0f;
	SpringArmLength_TurboFlightModeMultiplier  = 1.5f;
	CameraNormalFlightFOV        = 90.0f;
	CameraTurboFlightFOV         = 100.0f;
	CameraZoomSpeed              = 10.0f;
	CameraFOVZoomSpeed           = 5.0f;
	Name                         = FText::FromString("Unnamed Player");
	Faction                      = ESpacecraftFaction::Human;

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

void ASpacePlayerPawn::OnDamageTaken(ASpacecraftPawn * DamageCauser)
{
	// TODO: add some hit effects perhaps?
}

// TODO: W.I.P.
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
		}, 0.75f, false);
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

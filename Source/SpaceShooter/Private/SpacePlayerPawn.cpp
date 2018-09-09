// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacePlayerPawn.h"
#include "Weapon.h"
#include "Projectile.h"

#include "GameFramework/SpringArmComponent.h"

#include "Components/StaticMeshComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Camera/CameraComponent.h"


/** Sets default values. */
ASpacePlayerPawn::ASpacePlayerPawn()
{
	// Initialize components.
	CameraComponent          = CreateDefaultSubobject<UCameraComponent   >("Camera Component");
	SpringArmComponent       = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	
	MoveForwardMaxTurboSpeed = 1600.0f;
	MoveForwardMaxSpeed      = 1200.0f;
	MoveForwardSpeed         = MoveForwardMaxSpeed;
	MoveBackwardSpeed        = 900.0f;
	SpringArmOffset          = FVector(-500.0f, 0.0f, 600.0f);
	SpringArmRotation        = FRotator(-50.0f, 0.0f, 0.0f);
	SpringArmTurboLength     = 100.0f;
	SpacecraftTurnSpeed      = 10.0f;

	// SpringArmComponent setup:
	SpringArmComponent->SetupAttachment(CentralSceneComponent);
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
	
}

void ASpacePlayerPawn::OnTurboModeActivated()
{
	SpringArmComponent->TargetArmLength = SpringArmTurboLength;
}

void ASpacePlayerPawn::OnTurboModeDeactivated()
{
	SpringArmComponent->TargetArmLength = 0.0f;
}

void ASpacePlayerPawn::BeginFiringPrimaryWeapons()
{
	Super::BeginFiringPrimaryWeapons();

}

void ASpacePlayerPawn::EndFiringPrimaryWeapons()
{
	Super::EndFiringPrimaryWeapons();

}

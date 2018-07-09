// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacePlayerPawn.h"

#include "ConstructorHelpers.h"

#include "Engine/World.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "Kismet/KismetMathLibrary.h"

#include "Components/StaticMeshComponent.h"

#include "Camera/CameraComponent.h"


/** Sets default values. */
ASpacePlayerPawn::ASpacePlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initialize components.
	SpaceshipMeshComponent        = CreateDefaultSubobject<UStaticMeshComponent>("Spaceship Mesh Component");
	CentralPlayerSceneComponent   = CreateDefaultSubobject<USceneComponent>("Central Player Scene Component");
	SpringArmComponent            = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
	CameraComponent               = CreateDefaultSubobject<UCameraComponent>("Camera Component");
	SpaceshipMovementComponent    = CreateDefaultSubobject<UFloatingPawnMovement>("Spaceship Movement Component");

	RootComponent = SpaceshipMeshComponent;

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
	SpringArmComponent->SetRelativeLocation(FVector(-500.0f, 0.0f, 600.0f));
	SpringArmComponent->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bEnableCameraLag = true;						// Enable spring arm lag.
	SpringArmComponent->bEnableCameraRotationLag = false;				// Disable spring arm rotation lag. (camera never rotates)
	SpringArmComponent->CameraLagSpeed = 20.0f;
	// ~ end of SpringArmComponent setup.

	// CameraComponent setup:
	CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);
	CameraComponent->bUsePawnControlRotation = false;					// Do not rotate the camera along with the player.
	// ~ end of CameraComponent setup.

	// FloatingPawnMovementComponent setup:
	SpaceshipMovementComponent->UpdatedComponent = SpaceshipMeshComponent;
	// ~ end of FloatingPawnMovementComponent setup.
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

	// Keep the helper scene component's rotation at a fixed zero value on all axis so the
	//		camera's spring arm (that is attached to it) never rotates its children (i.e. the camera).
	CentralPlayerSceneComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void ASpacePlayerPawn::RotateShip(FRotator rotator)
{
	// TODO: should turn the InterpSpeed param into a class variable.
	FRotator finalRotation = UKismetMathLibrary::RInterpTo(SpaceshipMeshComponent->GetComponentRotation(), rotator, FApp::GetDeltaTime(), 10.0f);
	
	SpaceshipMeshComponent->SetWorldRotation(finalRotation);
}

void ASpacePlayerPawn::MoveForward(float Value)
{
	if (Value > 0.0f)
	{
		// Add movement in the correct direction.
		
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASpacePlayerPawn::MoveBackward(float Value)
{
	if (Value < 0.0f)
	{
		// Add movement in the correct direction.
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

UPawnMovementComponent * ASpacePlayerPawn::GetMovementComponent() const
{
	return SpaceshipMovementComponent;
}

void ASpacePlayerPawn::RotateShipClockwise()
{

}

void ASpacePlayerPawn::RotateShipCounterclockwise()
{

}

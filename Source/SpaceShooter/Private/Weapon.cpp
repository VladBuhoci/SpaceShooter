// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Weapon.h"
#include "Projectile.h"
#include "SpacecraftPawn.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Public/TimerManager.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent           = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	Damage                  = 10.0f;
	SpreadAngle             = 30.0f;
	Accuracy                = 75.0f;
	AccuracyRecoveryRate    = 3.0f;
	AccuracyRecoveryDelay   = 0.5f;
	FireRate                = 5.0f;
	Recoil                  = 1.5f;
	RecoilReduction         = 0.5f;
	TimePassedSinceLastShot = 0.0f;
}

/** Called when the game starts or when spawned. */
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAccuracy = Accuracy;
}

/** Called every frame. */
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimePassedSinceLastShot += DeltaTime;
	TimePassedSinceLastShot = FMath::Clamp(TimePassedSinceLastShot, TimePassedSinceLastShot, 100.0f);	// Don't let this go too high..
}

void AWeapon::FireWeapon(ASpacecraftPawn* ProjectileOwner)
{
	if (ProjectileClass && IsAllowedToFireWeapon())
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// Set up the spawn parameters for the projectile that'll be fired.
			float InaccuracyFactorRaw          = SpreadAngle - (CurrentAccuracy * SpreadAngle / 100);			// Formula used: SpreadAngle - Accuracy % SpreadAngle.
			float InaccuracyFactorMax          = FMath::Clamp(InaccuracyFactorRaw, 0.0f, SpreadAngle);
			float InaccuracyFactor             = FMath::FRandRange(-InaccuracyFactorMax, InaccuracyFactorMax);

			FTransform ProjectileTransform;
			FVector    ProjectileLocation = MeshComponent->GetSocketLocation("Muzzle");
			FRotator   ProjectileRotation = GetActorRotation();

			ProjectileRotation.Yaw += InaccuracyFactor;

			ProjectileTransform.SetLocation(ProjectileLocation);
			ProjectileTransform.SetRotation(ProjectileRotation.Quaternion());
			
			// Spawn the projectile.
			AProjectile* FiredProjectile = World->SpawnActor<AProjectile>(ProjectileClass, ProjectileTransform);
			
			if (FiredProjectile)
			{
				// If we fire something, abort the countdown process that would trigger accuracy recovery.
				GetWorldTimerManager().ClearTimer(CountToBeginAccuracyRecoveryTimer);

				FiredProjectile->SetProjectileOwner(ProjectileOwner);
				FiredProjectile->SetDamage(Damage);

				PlayWeaponFiringEffects();
				ApplyRecoil();
				// TODO: activate a timer based on accuracy recovery delay field that activates another timer
				//		which brings current accuracy back to base value using an accuracy recovery rate field.

				// Reset the counter for time which had passed between shots.
				ResetTimeSinceLastWeaponUsage();

				GetWorldTimerManager().SetTimer(CountToBeginAccuracyRecoveryTimer, this, &AWeapon::RecoverAccuracyByOneWholeRate, AccuracyRecoveryDelay);
			}
		}
	}
}

void AWeapon::ResetTimeSinceLastWeaponUsage()
{
	TimePassedSinceLastShot = 0.0f;
}

bool AWeapon::IsAllowedToFireWeapon()
{
	return TimePassedSinceLastShot >= 1.0f / FireRate;
}

// TODO: perhaps we can add some recoil effects too? (camera shake, ship knock back etc.)
void AWeapon::PlayWeaponFiringEffects()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		// Play destruction sound.
		if (WeaponFiringSound)
		{
			// TODO: The location used for playing the sound should be indicated by some basic Actor Component.
			UGameplayStatics::PlaySoundAtLocation(WorldPtr, WeaponFiringSound, this->GetActorLocation());
		}

		// Spawn the destruction effect particles.
		if (WeaponFiringParticleEffect != NULL)
		{
			// TODO: The location used for spawning the emitter should be indicated by some basic Actor Component.
			UGameplayStatics::SpawnEmitterAtLocation(WorldPtr, WeaponFiringParticleEffect, this->GetActorLocation());
		}
	}
}

void AWeapon::ApplyRecoil()
{
	CurrentAccuracy -= Recoil - RecoilReduction;
	CurrentAccuracy = FMath::Clamp(CurrentAccuracy, 0.0f, 100.0f);
}

void AWeapon::RecoverAccuracyByOneWholeRate()
{
	// We try to get back to the base value of the accuracy here.

	float Delta = FMath::Abs(CurrentAccuracy - Accuracy);

	if (Delta == 0)
		return;

	int TimeUntilNextRecovery = 1;		// 1 second until we recover some more accuracy units.
	int SignFactor = CurrentAccuracy == Accuracy ? 0 : CurrentAccuracy < Accuracy ? 1 : -1;

	if (Delta < AccuracyRecoveryRate)
	{
		CurrentAccuracy += Delta * SignFactor;
		CurrentAccuracy = FMath::Clamp(CurrentAccuracy, 0.0f, 100.0f);
	}
	else
	{
		CurrentAccuracy += AccuracyRecoveryRate * SignFactor;
		CurrentAccuracy = FMath::Clamp(CurrentAccuracy, 0.0f, 100.0f);
	}

	if (CurrentAccuracy != Accuracy)
		GetWorldTimerManager().SetTimer(CountToBeginAccuracyRecoveryTimer, this, &AWeapon::RecoverAccuracyByOneWholeRate, TimeUntilNextRecovery);
}

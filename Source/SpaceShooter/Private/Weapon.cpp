// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Weapon.h"
#include "Projectile.h"
#include "SpacecraftPawn.h"
#include "SpaceEnums.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Public/TimerManager.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent            = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	Type                     = EWeaponType::Blaster;
	Damage                   = 23.0f;
	SpreadAngle              = 12.5f;
	Accuracy                 = 85.0f;
	AccuracyRecoveryRate     = 21.3f;
	AccuracyRecoveryDelay    = 0.8f;
	bIsAccuracyBeingRestored = false;
	FireRate                 = 3.5f;
	Recoil                   = 4.0f;
	TimePassedSinceLastShot  = 0.0f;
	CurrentHeat              = 0.0f;
	HeatProducedPerShot      = 14.0f;
	CoolingRate              = 23.6f;
	bIsOverheated            = false;
	AmmoPerShot              = 1;
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

	CheckAccuracyStatus(DeltaTime);
	CheckHeatState(DeltaTime);
}

void AWeapon::FireWeapon(ASpacecraftPawn* ProjectileOwner, int32 & AmmoToUse)
{
	if (ProjectileClass && IsAllowedToFireWeapon() && AmmoPerShot <= AmmoToUse)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// Set up the spawn parameters for the projectile that'll be fired.
			float InaccuracyFactorRaw = SpreadAngle - (CurrentAccuracy * SpreadAngle / 100);			// Formula used: SpreadAngle - Accuracy % SpreadAngle.
			float InaccuracyFactorMax = FMath::Clamp(InaccuracyFactorRaw, 0.0f, SpreadAngle);
			float InaccuracyFactor    = FMath::FRandRange(-InaccuracyFactorMax, InaccuracyFactorMax);

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
				StopAccuracyRecoveryProcess();

				FiredProjectile->SetProjectileOwner(ProjectileOwner);
				FiredProjectile->SetDamage(Damage);

				PlayWeaponFiringEffects();
				ApplyRecoil();
				ProduceHeat();
				ConsumeAmmoForOneShot(AmmoToUse);
				
				// Reset the counter for time which had passed between shots.
				ResetTimeSinceLastWeaponUsage();

				// Prepare for future accuracy recovery.
				ScheduleAccuracyRecoveryProcess();
			}
		}
	}
}

void AWeapon::SetVisibility(bool State)
{
	MeshComponent->SetVisibility(State, true);
}

void AWeapon::ResetTimeSinceLastWeaponUsage()
{
	TimePassedSinceLastShot = 0.0f;
}

bool AWeapon::IsAllowedToFireWeapon()
{
	return !IsOverheated() && TimePassedSinceLastShot >= 1.0f / FireRate;
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
	CurrentAccuracy -= Recoil;
	CurrentAccuracy = FMath::Clamp(CurrentAccuracy, 0.0f, 100.0f);
}

void AWeapon::CheckAccuracyStatus(float DeltaTime)
{
	if (bIsAccuracyBeingRestored)
	{
		RecoverAccuracy(DeltaTime);
	}
}

void AWeapon::ScheduleAccuracyRecoveryProcess()
{
	GetWorldTimerManager().SetTimer(CountToBeginAccuracyRecoveryTimer, this, &AWeapon::BeginAccuracyRecoveryProcess, AccuracyRecoveryDelay);
}

void AWeapon::BeginAccuracyRecoveryProcess()
{
	bIsAccuracyBeingRestored = true;

	GetWorldTimerManager().ClearTimer(CountToBeginAccuracyRecoveryTimer);
}

void AWeapon::StopAccuracyRecoveryProcess()
{
	bIsAccuracyBeingRestored = false;

	GetWorldTimerManager().ClearTimer(CountToBeginAccuracyRecoveryTimer);
}

void AWeapon::RecoverAccuracy(float DeltaTime)
{
	CurrentAccuracy = FMath::FInterpConstantTo(CurrentAccuracy, Accuracy, DeltaTime, AccuracyRecoveryRate);

	// If current accuracy is equal to the initial value, stop the recovery process.
	if (CurrentAccuracy == Accuracy)
	{
		StopAccuracyRecoveryProcess();
	}
}

void AWeapon::CheckHeatState(float DeltaTime)
{
	if (CurrentHeat > 0)
	{
		CoolDown(DeltaTime);
	}
}

void AWeapon::ProduceHeat()
{
	CurrentHeat += HeatProducedPerShot;
	CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, 100.0f);

	if (CurrentHeat == 100.0f && !IsOverheated())
	{
		EnterOverheatedState();
	}
}

void AWeapon::EnterOverheatedState()
{
	bIsOverheated = true;
}

void AWeapon::ExitOverheatedState()
{
	bIsOverheated = false;
}

void AWeapon::CoolDown(float DeltaTime)
{
	CurrentHeat = FMath::FInterpConstantTo(CurrentHeat, 0.0f, DeltaTime, CoolingRate);

	// If CurrentHeat is equal to 0, stop the cooling process.
	if (CurrentHeat == 0.0f && IsOverheated())
	{
		ExitOverheatedState();
	}
}

void AWeapon::ConsumeAmmoForOneShot(int32 & AmmoToUse)
{
	AmmoToUse -= AmmoPerShot;
}

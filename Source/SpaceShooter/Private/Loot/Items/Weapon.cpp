// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Items/Weapon.h"
#include "Projectiles/Projectile.h"
#include "Pawns/SpacecraftPawn.h"
#include "Globals/SpaceEnums.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Public/TimerManager.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	Name                     = FText::FromString("Unnamed Weapon");
	MeshComponent            = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	Type                     = EWeaponType::Unknown;
	Damage                   = 23.0f;
	ProjectilesPerShot       = 1;
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
	if (ProjectileClass && IsAllowedToFireWeapon() && HasEnoughAmmoForOneShot(AmmoToUse))
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// Prepare transform helpers.
			FVector  ProjectileLocation = MeshComponent->GetSocketLocation("Muzzle");
			FRotator ProjectileRotation = GetActorRotation();

			// Set up the spawn parameters for the projectile that'll be fired.
			float InitialRotationDeviation = (-SpreadAngle) + (SpreadAngle / ProjectilesPerShot);
			float SpreadAnglePerProjectile = SpreadAngle * 2 / ProjectilesPerShot;		// Formula used: Cone aperture's angle / ProjectilesPerShot. [aperture angle = spread angle * 2]
			float InaccuracyFactorMax      = ComputeInaccuracyFactor(SpreadAnglePerProjectile);

			bool bProjectilesFired = false;

			for (int32 i = 0; i < ProjectilesPerShot; i++)
			{
				if (SpawnProjectile(World, ProjectileLocation, ProjectileRotation, ProjectileOwner, InitialRotationDeviation, -InaccuracyFactorMax, InaccuracyFactorMax))
				{
					bProjectilesFired = true;
				}

				// Update the initial deviation for the next projectile that spawns (creating some sort of radial effect).
				InitialRotationDeviation += SpreadAnglePerProjectile;
			}

			if (bProjectilesFired)
			{
				DoPostFiringTasks(AmmoToUse);
			}
		}
	}
}

void AWeapon::ProvideAttributes(TArray<FItemAttribute_Float> & AttributesArrayToSupply)
{
	AttributesArrayToSupply.Add(FItemAttribute_Float(FText::FromString("Damage"), Damage));
	AttributesArrayToSupply.Add(FItemAttribute_Float(FText::FromString("Accuracy"), Accuracy));
	AttributesArrayToSupply.Add(FItemAttribute_Float(FText::FromString("Fire Rate"), FireRate));
}

void AWeapon::SetVisibility(bool CurrentState)
{
	MeshComponent->SetVisibility(CurrentState, true);
}

float AWeapon::ComputeInaccuracyFactor(float ProjectileSpreadAngle)
{
	// Formula used: ProjectileSpreadAngle - Accuracy % ProjectileSpreadAngle.
	float InaccuracyFactorRaw = ProjectileSpreadAngle - (CurrentAccuracy * ProjectileSpreadAngle / 100);
	float InaccuracyFactorMax = FMath::Clamp(InaccuracyFactorRaw, 0.0f, ProjectileSpreadAngle);

	return InaccuracyFactorMax;
}

AProjectile* AWeapon::SpawnProjectile(UWorld* World, FVector & ProjectileLocation, FRotator & ProjectileRotationBase,
	ASpacecraftPawn* ProjectileOwner, float InitialRotationDeviation, float InaccuracyFactorMin, float InaccuracyFactorMax)
{
	float InaccuracyFactor = FMath::FRandRange(-InaccuracyFactorMax, InaccuracyFactorMax);

	FRotator ParticularProjectileRotation = ProjectileRotationBase;

	ParticularProjectileRotation.Yaw += InitialRotationDeviation;
	ParticularProjectileRotation.Yaw += InaccuracyFactor;

	// Spawn the projectile.
	AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, ProjectileLocation, ParticularProjectileRotation);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetProjectileOwner(ProjectileOwner);
		SpawnedProjectile->SetDamage(Damage);
	}

	return SpawnedProjectile;
}

void AWeapon::DoPostFiringTasks(int32 & AmmoToUse)
{
	StopAccuracyRecoveryProcess();

	PlayWeaponFiringEffects();
	ApplyRecoil();
	ProduceHeat();
	ConsumeAmmoForOneShot(AmmoToUse);

	// Reset the counter for time which had passed between shots.
	ResetTimeSinceLastWeaponUsage();

	// Prepare for future accuracy recovery.
	ScheduleAccuracyRecoveryProcess();
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

bool AWeapon::HasEnoughAmmoForOneShot(const int32 & AmmoToUse) const
{
	return AmmoPerShot <= AmmoToUse;
}

void AWeapon::ConsumeAmmoForOneShot(int32 & AmmoToUse)
{
	AmmoToUse -= AmmoPerShot;
}

void AWeapon::SetBarrelMesh(UStaticMesh* Mesh)
{
	if (Mesh)
	{
		// TODO: set and snap to body
	}
}

void AWeapon::SetBodyMesh(UStaticMesh* Mesh)
{
	if (Mesh)
	{
		MeshComponent->SetStaticMesh(Mesh);
		// TODO: snap to grip
	}
}

void AWeapon::SetGripMesh(UStaticMesh* Mesh)
{
	if (Mesh)
	{
		// TODO: set and snap to ship
	}
}

void AWeapon::SetMaterial(UMaterialInterface* Mat)
{
	if (Mat)
	{
		// TODO: set the same material to all 3 meshes (barrel, body & grip).
		MeshComponent->SetMaterial(0, Mat);
	}
}

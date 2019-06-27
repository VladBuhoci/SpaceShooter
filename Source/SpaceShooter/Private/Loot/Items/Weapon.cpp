// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Loot/Items/Weapon.h"
#include "Loot/Creation/WeaponPool.h"
#include "Projectiles/Projectile.h"
#include "Pawns/SpacecraftPawn.h"
#include "Globals/SpaceEnums.h"
#include "Globals/SpaceStructs.h"
#include "GameModes/SpaceGameMode.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"

#include "Runtime/Engine/Public/TimerManager.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	Name                                   = FText::FromString("Unnamed Weapon");
	MeshComponent                          = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	Type                                   = EWeaponType::Unknown;

	WeaponAttributes.Damage                = 23;
	WeaponAttributes.ProjectilesPerShot    = 1;
	WeaponAttributes.SpreadAngle           = 12.5f;
	WeaponAttributes.Accuracy              = 85.0f;
	WeaponAttributes.AccuracyRecoveryRate  = 21.3f;
	WeaponAttributes.AccuracyRecoveryDelay = 0.8f;
	WeaponAttributes.FireRate              = 3.5f;
	WeaponAttributes.Recoil                = 4.0f;
	WeaponAttributes.HeatProducedPerShot   = 14.0f;
	WeaponAttributes.CoolingRate           = 23.6f;
	WeaponAttributes.AmmoPerShot           = 1;

	bIsAccuracyBeingRestored               = false;
	TimePassedSinceLastShot                = 0.0f;
	CurrentHeat                            = 0.0f;
	bIsOverheated                          = false;
}

/** Called when the game starts or when spawned. */
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAccuracy = WeaponAttributes.Accuracy;

	UWorld* WorldPtr = GetWorld();
	if (WorldPtr)
	{
		ASpaceGameMode* SpaceGameMode = Cast<ASpaceGameMode>(UGameplayStatics::GetGameMode(WorldPtr));
		if (SpaceGameMode)
		{
			GlobalWeaponPool = SpaceGameMode->GetGlobalWeaponPool();
		}
	}
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

void AWeapon::FireWeapon(ASpacecraftPawn* ProjectileOwner, FAmmunitionStock & AmmoStockToUse)
{
	if (ProjectileClass && IsAllowedToFireWeapon() && HasEnoughAmmoForOneShot(AmmoStockToUse))
	{
		UWorld* World = GetWorld();

		if (World)
		{
			// Prepare transform helpers.
			FVector  ProjectileLocation = MeshComponent->GetSocketLocation("Muzzle");
			FRotator ProjectileRotation = GetActorRotation();

			// Set up the spawn parameters for the projectile that'll be fired.
			float InitialRotationDeviation = (-WeaponAttributes.SpreadAngle) + (WeaponAttributes.SpreadAngle / WeaponAttributes.ProjectilesPerShot);
			// Formula used: Cone aperture's angle / ProjectilesPerShot. [aperture angle = spread angle * 2]
			float SpreadAnglePerProjectile = WeaponAttributes.SpreadAngle * 2 / WeaponAttributes.ProjectilesPerShot;
			float InaccuracyFactorMax      = ComputeInaccuracyFactor(SpreadAnglePerProjectile);

			bool bProjectilesFired = false;

			for (int32 i = 0; i < WeaponAttributes.ProjectilesPerShot; i++)
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
				DoPostFiringTasks(AmmoStockToUse);
			}
		}
	}
}

void AWeapon::ProvideAttributes(TArray<FItemAttribute_Text> & AttributesArrayToSupply)
{
	if (!GlobalWeaponPool)
		return;

#define DescrOf(Attr) GlobalWeaponPool->GetPredefinedAttributeDescription(EWeaponAttribute::Attr)

	DECL_AND_DEF_ITEM_TEXT_ATTR_FROM_INT32_VAR(DamageAsText  , DescrOf(Damage)             , WeaponAttributes.Damage  )
	DECL_AND_DEF_ITEM_TEXT_ATTR_FROM_FLOAT_VAR(AccuracyAsText, DescrOf(Accuracy)           , WeaponAttributes.Accuracy)
	DECL_AND_DEF_ITEM_TEXT_ATTR_FROM_FLOAT_VAR(FireRateAsText, DescrOf(FireRate)           , WeaponAttributes.FireRate)
	DECL_AND_DEF_ITEM_TEXT_ATTR_FROM_FLOAT_VAR(HeatAsText    , DescrOf(HeatProducedPerShot), WeaponAttributes.HeatProducedPerShot)

	// Combine ProjectilesPerShot and Damage properties as such: Damage X ProjectilesPerShot.
	if (WeaponAttributes.ProjectilesPerShot > 1)
	{
		DamageAsText.Value = FText::FromString(DamageAsText.Value.ToString() + " x" + FString::FromInt(WeaponAttributes.ProjectilesPerShot));
	}

	AttributesArrayToSupply.Add(DamageAsText);
	AttributesArrayToSupply.Add(AccuracyAsText);
	AttributesArrayToSupply.Add(FireRateAsText);
	AttributesArrayToSupply.Add(HeatAsText);
	
	if (WeaponAttributes.AmmoPerShot > 1)
	{
		DECL_AND_DEF_ITEM_TEXT_ATTR_FROM_INT32_VAR(AmmoPerShotAsText, DescrOf(AmmoPerShot), WeaponAttributes.AmmoPerShot)

		AttributesArrayToSupply.Add(AmmoPerShotAsText);
	}

#undef DescrOf
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
		SpawnedProjectile->SetDamage(WeaponAttributes.Damage);
	}

	return SpawnedProjectile;
}

void AWeapon::DoPostFiringTasks(FAmmunitionStock & AmmoStockToUse)
{
	StopAccuracyRecoveryProcess();

	PlayWeaponFiringEffects();
	ApplyRecoil();
	ProduceHeat();
	ConsumeAmmoForOneShot(AmmoStockToUse);

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
	return !IsOverheated() && TimePassedSinceLastShot >= 1.0f / WeaponAttributes.FireRate;
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
	CurrentAccuracy -= WeaponAttributes.Recoil;
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
	GetWorldTimerManager().SetTimer(CountToBeginAccuracyRecoveryTimer, this, &AWeapon::BeginAccuracyRecoveryProcess,
		WeaponAttributes.AccuracyRecoveryDelay);
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
	CurrentAccuracy = FMath::FInterpConstantTo(CurrentAccuracy, WeaponAttributes.Accuracy, DeltaTime, WeaponAttributes.AccuracyRecoveryRate);

	// If current accuracy is equal to the initial value, stop the recovery process.
	if (CurrentAccuracy == WeaponAttributes.Accuracy)
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
	CurrentHeat += WeaponAttributes.HeatProducedPerShot;
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
	CurrentHeat = FMath::FInterpConstantTo(CurrentHeat, 0.0f, DeltaTime, WeaponAttributes.CoolingRate);

	// If CurrentHeat is equal to 0, stop the cooling process.
	if (CurrentHeat == 0.0f && IsOverheated())
	{
		ExitOverheatedState();
	}
}

bool AWeapon::HasEnoughAmmoForOneShot(const FAmmunitionStock & AmmoStockToUse) const
{
	return AmmoStockToUse.bEndlessAmmo || WeaponAttributes.AmmoPerShot <= AmmoStockToUse.CurrentAmmoQuantity;
}

void AWeapon::ConsumeAmmoForOneShot(FAmmunitionStock & AmmoStockToUse)
{
	AmmoStockToUse.CurrentAmmoQuantity -= WeaponAttributes.AmmoPerShot;
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

void AWeapon::SetNumericAttributes(FWeaponAttributes & Attributes)
{
	WeaponAttributes = Attributes;

	// Sanity checks.

	WeaponAttributes.Accuracy = FMath::Clamp(WeaponAttributes.Accuracy, 0.0f, 100.0f);
}

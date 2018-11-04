// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Weapon.h"
#include "Projectile.h"
#include "SpacecraftPawn.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"

#include "Kismet/GameplayStatics.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent           = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	Damage                  = 10.0f;
	FireRate                = 5.0f;
	TimePassedSinceLastShot = 0.0f;
}

/** Called when the game starts or when spawned. */
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

/** Called every frame. */
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimePassedSinceLastShot += DeltaTime;
}

void AWeapon::FireWeapon(ASpacecraftPawn* ProjectileOwner)
{
	if (ProjectileClass && IsAllowedToFireWeapon())
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FTransform ProjectileTransform;
			
			ProjectileTransform.SetLocation(GetActorLocation() + GetActorForwardVector() * 100);
			ProjectileTransform.SetRotation(GetActorRotation().Quaternion());
			
			AProjectile* FiredProjectile = World->SpawnActor<AProjectile>(ProjectileClass, ProjectileTransform);
			
			if (FiredProjectile)
			{
				FiredProjectile->SetProjectileOwner(ProjectileOwner);
				FiredProjectile->SetDamage(Damage);

				// TODO: Set up more stuff for it.
				
				PlayWeaponFiringEffects();

				// Reset the counter.
				ResetTimeSinceLastWeaponUsage();
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


// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Weapon.h"
#include "Projectile.h"

#include "Components/StaticMeshComponent.h"

#include "Engine/World.h"


/** Sets default values. */
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent           = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
	FireRate                = 0.0f;
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

void AWeapon::FireWeapon(ESpacecraftFaction OwnerType)
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
				FiredProjectile->SetOwnerType(OwnerType);
				//FiredProjectile->SetDamage(0.0f);
				
				// TODO: Set up more stuff for it.

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


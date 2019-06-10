// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Projectiles/Projectile.h"
#include "Pawns/SpacecraftPawn.h"

#include "Components/StaticMeshComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"

#include "Particles/ParticleSystemComponent.h"

#include "Kismet/GameplayStatics.h"


/** Sets default values. */
AProjectile::AProjectile()
{
	MeshComponent               = CreateDefaultSubobject<UStaticMeshComponent        >("Mesh Component");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");
	TrailingParticleEmitter     = CreateDefaultSubobject<UParticleSystemComponent    >("Trailing Particles Emitter");

	RootComponent               = MeshComponent;
	InitialLifeSpan             = 5.0f;

	Damage                      = 0.0f;
	
	// MeshComponent setup:
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	// ...
	MeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::ExecuteOnProjectileBeginOverlap);
	// ~ end of MeshComponent setup.

	// ProjectileMovementComponent setup:
	ProjectileMovementComponent->InitialSpeed           = 3000.0f;
	ProjectileMovementComponent->MaxSpeed               = 3000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	// ...
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectile::ExecuteOnProjectileBounce);
	ProjectileMovementComponent->OnProjectileStop  .AddDynamic(this, &AProjectile::ExecuteOnProjectileStop);
	// ~ end of ProjectileMovementComponent setup.

	// TrailingParticleEmitter setup:
	TrailingParticleEmitter->SetupAttachment(MeshComponent);
	// ~ end of TrailingParticleEmitter setup.
}

/** Called when the game starts or when spawned. */
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void AProjectile::ExecuteOnProjectileBounce_Implementation(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	//UE_LOG(LogTemp, Log, TEXT("Projectile bounced!"));
}

void AProjectile::ExecuteOnProjectileStop_Implementation(const FHitResult& ImpactResult)
{
	//UE_LOG(LogTemp, Log, TEXT("Projectile stopped!"));
}

void AProjectile::ExecuteOnProjectileBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// We want to overlap spacecrafts and projectiles that belong to the same "faction" and hit everything else.
	if (OtherActor != NULL && OtherActor != this)
	{
		if (this->ProjectileOwner)
		{
			// TODO: we might have issues with projectiles hitting something too early, when the OwnerType (faction) isn't set yet.
			// Temporary hack (?):
			if (this->ProjectileOwner->GetFaction() == ESpacecraftFaction::Unspecified)
			{
				// By returning, we skip any Hit/Destroy operations.
				return;
			}

			// Check if we hit another projectile.
			if (AProjectile* OtherProjectile = Cast<AProjectile>(OtherActor))
			{
				if (OtherProjectile->ProjectileOwner)
				{
					// Destroy this projectile if it collides with another one belonging to the enemy side.
					if (OtherProjectile->ProjectileOwner->GetFaction() != this->ProjectileOwner->GetFaction())
					{
						this->DestroyProjectile();
					}

					return;
				}
			}

			// Check if we hit another spacecraft's ship mesh.
			if (ASpacecraftPawn* OtherSpacecraft = Cast<ASpacecraftPawn>(OtherActor))
			{
				if (UStaticMeshComponent* OtherSpacecraftMesh = Cast<UStaticMeshComponent>(OtherComp))
				{
					// Destroy this projectile if it collides with a spacecraft belonging to the enemy side,
					//		but not before applying damage to it.
					if (OtherSpacecraft->GetFaction() != this->ProjectileOwner->GetFaction())
					{
						// TODO:
						UGameplayStatics::ApplyDamage(OtherActor, Damage, ProjectileOwner->GetController(), ProjectileOwner, UDamageType::StaticClass());

						this->DestroyProjectile();
					}
				}

				return;
			}
		}
	}
}

void AProjectile::DestroyProjectile()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		// Play impact sound.
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldPtr, ImpactSound, this->GetActorLocation());
		}

		// Spawn the impact effect particles.
		if (ImpactParticleEffect != NULL)
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldPtr, ImpactParticleEffect, this->GetActorLocation());
		}
	}

	// Finally, remove this projectile from the world.
	Destroy();
}

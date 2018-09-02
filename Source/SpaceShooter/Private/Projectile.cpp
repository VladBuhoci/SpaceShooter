// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Projectile.h"

#include "Components/StaticMeshComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "Particles/ParticleSystemComponent.h"

#include "Kismet/GameplayStatics.h"


/** Sets default values. */
AProjectile::AProjectile()
{
	MeshComponent               = CreateDefaultSubobject<UStaticMeshComponent        >("Mesh Component");
	TrailingParticleEmitter     = CreateDefaultSubobject<UParticleSystemComponent    >("Trailing Particles Emitter");
	DestroyParticleEffect       = CreateDefaultSubobject<UParticleSystemComponent    >("Destroy Particles Emitter");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Component");

	RootComponent               = MeshComponent;
	InitialLifeSpan             = 5.0f;

	Damage                      = 0.0f;
	OwnerType                   = EProjectileOwnerType::Unspecified;
	
	// MeshComponent setup:
	MeshComponent->OnComponentHit.AddDynamic(this, &AProjectile::ExecuteOnProjectileHit);
	// ~ end of MeshComponent setup.

	// TrailingParticleEmitter setup:
	TrailingParticleEmitter->SetupAttachment(MeshComponent);
	// ~ end of TrailingParticleEmitter setup.

	// DestroyParticleEffect setup:
	DestroyParticleEffect->SetupAttachment(MeshComponent);
	DestroyParticleEffect->bAutoActivate = false;
	// ~ end of DestroyParticleEffect setup.

	// ProjectileMovementComponent setup:
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed               = 3000.0f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectile::ExecuteOnProjectileBounce);
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::ExecuteOnProjectileStop);
	// ~ end of ProjectileMovementComponent setup.
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

void AProjectile::ExecuteOnProjectileHit_Implementation(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this)
	{
		// Check if we hit another projectile.
		AProjectile* OtherProjectile = Cast<AProjectile>(OtherActor);
		
		if (OtherProjectile)
		{
			// Destroy both projectiles if their owners belong to different factions.
			if (OtherProjectile->GetOwnerType() != this->GetOwnerType())
			{
				OtherProjectile->DestroyProjectile();

				this->DestroyProjectile();
			}
		}
	}
}

void AProjectile::DestroyProjectile()
{
	UWorld* WorldPtr = GetWorld();

	// Play impact sound.
	if (ImpactSound && WorldPtr)
	{
		UGameplayStatics::PlaySoundAtLocation(WorldPtr, ImpactSound, this->GetActorLocation());
	}

	// Activate the destruction effect (particle system).
	if (DestroyParticleEffect != NULL)
	{
		// @TODO: this should be moved somewhere else because destroying the Actor will destroy the particle emitter too.
		DestroyParticleEffect->ActivateSystem();
	}

	// Finally, remove this projectile from the world.
	Destroy();
}

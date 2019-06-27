// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Effects/Explosion.h"

#include "PhysicsEngine/RadialForceComponent.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Kismet/GameplayStatics.h"

#include "Engine/World.h"


AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialLifeSpan = 7.0f;

	ExplosionRadialForceComponent = CreateDefaultSubobject<URadialForceComponent>("Explosion Radial Force Component");

	ExplosionRadialForceComponent->SetupAttachment(RootComponent);
	ExplosionRadialForceComponent->Radius = 300.0f;
	ExplosionRadialForceComponent->ForceStrength = 75.0f;
	ExplosionRadialForceComponent->ImpulseStrength = 75.0f;
	ExplosionRadialForceComponent->Falloff = ERadialImpulseFalloff::RIF_Linear;
}

void AExplosion::PlayEffects()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		// Play destruction sound.
		if (ExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(WorldPtr, ExplosionSound, this->GetActorLocation());
		}

		// Spawn the destruction effect particles.
		if (ExplosionParticleEffect != NULL)
		{
			UGameplayStatics::SpawnEmitterAtLocation(WorldPtr, ExplosionParticleEffect, this->GetActorLocation());
		}

		// Simulate the explosion's physical force to push physics based objects in different directions.
		ExplosionRadialForceComponent->FireImpulse();

		OnAfterEffects();
	}
}

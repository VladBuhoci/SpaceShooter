// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpacecraftExplosion.generated.h"

// Forward declarations.
class URadialForceComponent;


/**
 * Class that handles explosion effects such as particles, sounds and radial forces used to push physics-enabled objects.
 */
UCLASS()
class SPACESHOOTER_API ASpacecraftExplosion : public AActor
{
	GENERATED_BODY()

	/** Particle system which is spawned to represent the explosion, visually. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft Explosion", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ExplosionParticleEffect;

	/** Sound of the explosion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft Explosion", Meta = (AllowPrivateAccess = "true"))
	USoundBase* ExplosionSound;

	/**
	 * Simulated radial force of the explosion.
	 * Useful for pushing items with physics enabled (like loot chests) in random directions.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft Explosion", Meta = (AllowPrivateAccess = "true"))
	URadialForceComponent* ExplosionRadialForceComponent;

	/** Time to live for this actor, measured in seconds. Once expired, the actor is destroyed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft Explosion", Meta = (AllowPrivateAccess = "true", ClampMin = "5", UIMin = "5", ClampMax = "15", UIMax = "15"))
	float LifeSpan;
	
public:	
	/** Sets default values. */
	ASpacecraftExplosion();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:	
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

private:
	void PlayEffects();
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// Forward declarations.
class UProjectileMovementComponent;
class UParticleSystemComponent;


/**
 * Base class of all projectiles shot by spaceships.
 */
UCLASS()
class SPACESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

private:
	/** Mesh component of this projectile. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;

	/** Movement component of this projectile. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	/** The particles left behind by the projectile. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* TrailingParticleEmitter;

	/** Particle system which is spawned when this projectile hits a surface (and gets destroyed). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticleEffect;

	/** The owner of this projectile (spacecraft that shot it). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	ASpacecraftPawn* ProjectileOwner;
	
	/** Damage dealt by this projectile. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	float Damage;

	/** Sound played when this projectile hits a surface. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", Meta = (AllowPrivateAccess = "true"))
	USoundBase* ImpactSound;
	
public:
	/** Sets default values for this actor's properties. */
	AProjectile();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called whenever the projectile bounces off a surface. */
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile", Meta = (DisplayName = "OnProjectileBounce"))
	void ExecuteOnProjectileBounce               (const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	void ExecuteOnProjectileBounce_Implementation(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/** Called when the projectile stops moving. */
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile", Meta = (DisplayName = "OnProjectileStop"))
	void ExecuteOnProjectileStop               (const FHitResult& ImpactResult);
	void ExecuteOnProjectileStop_Implementation(const FHitResult& ImpactResult);

	/** Called when the projectile overlaps something. */
	UFUNCTION(BlueprintNativeEvent, Category = "Projectile", Meta = (DisplayName = "OnProjectileHit"))
	void ExecuteOnProjectileBeginOverlap               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ExecuteOnProjectileBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	/** Triggers a few effects (sounds, particles) and then destroys the projectile. */
	void DestroyProjectile();


	/**********************************
				GETTERS
	**********************************/

public:
	/** Gets the mesh component of this projectile. */
	UFUNCTION(BlueprintPure, Category = "Projectile")
	UStaticMeshComponent* GetMesh()	const { return MeshComponent; }

	/** Gets the current damage value of this projectile. */
	UFUNCTION(BlueprintPure, Category = "Projectile")
	float GetDamage() const { return Damage; }

	/** Gets the type of owner that fired this projectile. */
	UFUNCTION(BlueprintPure, Category = "Projectile")
	ASpacecraftPawn* GetProjectileOwner() const { return ProjectileOwner; }

	/** Gets the projectile movement component of this projectile. */
	UFUNCTION(BlueprintPure, Category = "Projectile")
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovementComponent; }
	

	/**********************************
				SETTERS
	**********************************/

public:
	/** Sets a new value for the damage of this projectile. */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetDamage(float NewValue) { Damage = NewValue; }

	/** Sets a new owner type for this projectile. */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetProjectileOwner(ASpacecraftPawn* NewOwner) { ProjectileOwner = NewOwner; }
};

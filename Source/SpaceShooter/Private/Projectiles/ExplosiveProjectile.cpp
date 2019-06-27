// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Projectiles/ExplosiveProjectile.h"
#include "Pawns/SpacecraftPawn.h"

#include "Kismet/GameplayStatics.h"

#include "GameFramework/DamageType.h"


AExplosiveProjectile::AExplosiveProjectile()
{
	Damage          = 35;
	MinimumDamage   = 5;
	MaxDamageRadius = 100.0f;
	MinDamageRadius = 200.0f;
	DamageFalloff   = 5.0f;
}

void AExplosiveProjectile::ApplyDamage(ASpacecraftPawn * HitPawn)
{
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, GetActorLocation(),
		MaxDamageRadius, MinDamageRadius, DamageFalloff, UDamageType::StaticClass(), TArray<AActor*>(),
		DamageCauser, EventInstigator);
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "Projectiles/BasicProjectile.h"
#include "Pawns/SpacecraftPawn.h"

#include "Kismet/GameplayStatics.h"

#include "GameFramework/DamageType.h"


ABasicProjectile::ABasicProjectile()
{
	Damage = 5;
}

void ABasicProjectile::ApplyDamage(ASpacecraftPawn* HitPawn)
{
	UGameplayStatics::ApplyDamage(HitPawn, Damage, ProjectileOwner->GetController(), ProjectileOwner, UDamageType::StaticClass());
}

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "GameModes/SpaceGameMode.h"
#include "Pawns/SpacePlayerPawn.h"
#include "Pawns/SpaceEnemyPawn.h"
#include "Controllers/SpacePlayerController.h"
#include "UI/SpaceHUD.h"
#include "Loot/Creation/LootWeaponBuilder.h"
#include "Loot/Creation/WeaponPool.h"

#include "Engine/World.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


/** Sets default values. */
ASpaceGameMode::ASpaceGameMode()
{
	DefaultPawnClass      = ASpacePlayerPawn::StaticClass();
	PlayerControllerClass = ASpacePlayerController::StaticClass();
	HUDClass              = ASpaceHUD::StaticClass();
}

void ASpaceGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateGlobalWeaponPool();
	CreateLootBuilders();

	FindAllSpacecraftsInWorld();
	SupplyKnownSpacecraftsInWorld();
}

void ASpaceGameMode::OnSectorCleared()
{
	UE_LOG(LogTemp, Warning, TEXT("Sector cleared from enemies!"));
}

void ASpaceGameMode::OnPlayerDestroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Player spacecraft has been destroyed!"));
}

void ASpaceGameMode::NotifySpacecraftSpawned(ASpacecraftPawn* NewbornSpacecraft)
{
	if (!NewbornSpacecraft)
		return;

	if (!AllSpacecrafts.Contains(NewbornSpacecraft))
	{
		AllSpacecrafts.Add(NewbornSpacecraft);

		if (NewbornSpacecraft->GetClass()->IsChildOf(ASpaceEnemyPawn::StaticClass()))
			OnEnemySpacecraftSpawned(NewbornSpacecraft);

		SupplySpacecraftIfNeeded(NewbornSpacecraft);
	}
}

void ASpaceGameMode::NotifySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft)
{
	if (!DestroyedSpacecraft)
		return;

	if (AllSpacecrafts.Contains(DestroyedSpacecraft))
	{
		AllSpacecrafts.Remove(DestroyedSpacecraft);

		// Check if it's the player's spacecraft that got blown up.
		if (DestroyedSpacecraft->GetClass()->IsChildOf(ASpacePlayerPawn::StaticClass()))
		{
			OnPlayerDestroyed();
		}
		// Else check if there are any enemies left around (note that the player is also included in the array).
		else
		{
			OnEnemySpacecraftDestroyed(DestroyedSpacecraft);
			
			if (AllSpacecrafts.Num() == 1)
				OnSectorCleared();
		}
	}
}

void ASpaceGameMode::FindAllSpacecraftsInWorld()
{
	AllSpacecrafts.Empty();

	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		TArray<AActor*> ShipActors;

		UGameplayStatics::GetAllActorsOfClass(WorldPtr, ASpacecraftPawn::StaticClass(), ShipActors);

		if (ShipActors.Num() > 0)
		{
			for (AActor* ShipActor : ShipActors)
			{
				AllSpacecrafts.Add(Cast<ASpacecraftPawn>(ShipActor));
			}
		}
	}
}

void ASpaceGameMode::SupplyKnownSpacecraftsInWorld()
{
	for (auto Spacecraft : AllSpacecrafts)
	{
		SupplySpacecraftIfNeeded(Spacecraft);
	}
}

void ASpaceGameMode::SupplySpacecraftIfNeeded(ASpacecraftPawn* SpacecraftToSupply)
{
	if (!SpacecraftToSupply->HasAnyWeapons())
	{
		SupplySpacecraftWithStartingWeapons(SpacecraftToSupply, SpacecraftToSupply->GetStartingWeaponBlueprintTypes());

		SpacecraftToSupply->EquipWeaponFromSlot_Random();
	}
}

void ASpaceGameMode::SupplySpacecraftWithStartingWeapons(ASpacecraftPawn* SpacecraftToSupply, TArray<TSubclassOf<UItemBlueprint>> WeaponBlueprints)
{
	if (SpacecraftToSupply == nullptr || WeaponBlueprints.Num() == 0)
		return;

	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		for (auto WeaponBP : WeaponBlueprints)
		{
			ULootItemBuilder* ItemBuilder = GetLootBuilder(ULootWeaponBuilder::StaticClass());

			if (ItemBuilder)
			{
				AWeapon* Weapon = Cast<AWeapon>(ItemBuilder->Build(WeaponBP));

				if (Weapon)
				{
					SpacecraftToSupply->SupplyWeapon(Weapon);
				}
			}
		}
	}
}

void ASpaceGameMode::CreateGlobalWeaponPool()
{
	if (WeaponPoolClass)
	{
		WeaponPool = NewObject<UWeaponPool>(this, WeaponPoolClass);
	}
}

void ASpaceGameMode::CreateLootBuilders()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		for (TSubclassOf<ULootItemBuilder> BuilderClass : LootItemBuilderTypes)
		{
			if (BuilderClass)
			{
				ULootItemBuilder* NewBuilder = NewObject<ULootItemBuilder>(this, BuilderClass);

				if (NewBuilder)
				{
					LootItemBuilders.Add(BuilderClass, NewBuilder);
				}
			}
		}
	}
}

ULootItemBuilder* ASpaceGameMode::GetLootBuilder(TSubclassOf<ULootItemBuilder> Type) const
{
	return LootItemBuilders.Contains(Type) ? LootItemBuilders[Type] : nullptr;
}

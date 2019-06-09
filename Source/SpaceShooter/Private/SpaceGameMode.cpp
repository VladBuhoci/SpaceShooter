// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceGameMode.h"
#include "SpacePlayerPawn.h"
#include "SpacePlayerController.h"
#include "SpaceHUD.h"
#include "LootItemBuilder.h"

#include "Engine/World.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


/** Sets default values. */
ASpaceGameMode::ASpaceGameMode()
{
	DefaultPawnClass      = ASpacePlayerPawn::StaticClass();
	PlayerControllerClass = ASpacePlayerController::StaticClass();
	HUDClass              = ASpaceHUD::StaticClass();

	GenericItemBuilder    = CreateDefaultSubobject<ULootItemBuilder>("Generic Item Builder");
}

void ASpaceGameMode::BeginPlay()
{
	Super::BeginPlay();

	FindAllSpacecraftsInWorld();

	CreateLootBuilders();
}

void ASpaceGameMode::NotifySpacecraftSpawned(ASpacecraftPawn * NewbornSpacecraft)
{
	if (! AllSpacecrafts.Contains(NewbornSpacecraft))
	{
		AllSpacecrafts.Add(NewbornSpacecraft);
	}
}

void ASpaceGameMode::NotifySpacecraftDestroyed(ASpacecraftPawn* DestroyedSpacecraft)
{
	if (AllSpacecrafts.Contains(DestroyedSpacecraft))
	{
		AllSpacecrafts.Remove(DestroyedSpacecraft);
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
	return LootItemBuilders.Contains(Type) ? LootItemBuilders[Type] : GenericItemBuilder;
}

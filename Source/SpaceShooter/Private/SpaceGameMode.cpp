// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceGameMode.h"
#include "SpacePlayerPawn.h"
#include "SpacePlayerController.h"
#include "SpaceHUD.h"
#include "LootItemBuilder.h"

#include "Engine/World.h"


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

	CreateLootBuilders();
}

void ASpaceGameMode::CreateLootBuilders()
{
	UWorld* WorldPtr = GetWorld();

	if (WorldPtr)
	{
		for (TSubclassOf<ALootItemBuilder> BuilderClass : LootItemBuilderTypes)
		{
			if (BuilderClass)
			{
				ALootItemBuilder* NewBuilder = WorldPtr->SpawnActor<ALootItemBuilder>(BuilderClass, FTransform::Identity);

				if (NewBuilder)
				{
					LootItemBuilders.Add(BuilderClass, NewBuilder);
				}
			}
		}
	}
}

ALootItemBuilder* ASpaceGameMode::GetLootBuilder(TSubclassOf<ALootItemBuilder> Type) const
{
	return LootItemBuilders[Type];
}

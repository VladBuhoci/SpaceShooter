// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpaceGameMode.generated.h"

// Forward declarations.
class ALootItemBuilder;


/**
 * The class which defines the Space game rules and core actors that will spawn on map load.
 */
UCLASS()
class SPACESHOOTER_API ASpaceGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	/**
	 * Map of item builders. Since the game mode reference is available from nearly anywhere,
			it makes this map easy to access.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TMap<TSubclassOf<ALootItemBuilder>, ALootItemBuilder*> LootItemBuilders;

	/** Types of loot builders that will be spawned at game start. They will be accessible via this class. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space Game Mode", Meta = (AllowPrivateAccess = "true"))
	TSet<TSubclassOf<ALootItemBuilder>> LootItemBuilderTypes;

public:
	/** Sets default values. */
	ASpaceGameMode();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

private:
	void CreateLootBuilders();

	/**********************************
				GETTERS
	**********************************/

public:
	/** Given a type, returns the right loot item builder. */
	UFUNCTION(BlueprintPure, Category = "Loot Chest")
	ALootItemBuilder* GetLootBuilder(TSubclassOf<ALootItemBuilder> Type) const;
};

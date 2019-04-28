// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpacecraftPawn.h"
#include "SpaceEnemyPawn.generated.h"

// Forward declarations.
class USphereComponent;

class ALootChest;


USTRUCT(BlueprintInternalUseOnly)
struct FLootChestWithChance_KeyValue_Pair
{
	GENERATED_USTRUCT_BODY()

	/** Class of chest to spawn. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot")
	TSubclassOf<ALootChest> ChestType;

	/** Chance for this chest to be instantiated, as a percentage. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot")
	float SpawnChance;
};

/**
 * Base class of all Enemy spacecraft entities.
 */
UCLASS()
class SPACESHOOTER_API ASpaceEnemyPawn : public ASpacecraftPawn
{
	GENERATED_BODY()
	
private:
	/* Used to detect whenever a Spacecraft pawn enters or exits this spacecraft's "field of view". */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USphereComponent* DetectionArea;

	/* Used to detect whenever a Spacecraft pawn enters or exits this spacecraft's close quarters area. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USphereComponent* CloseQuartersArea;

	/** The targeted spacecraft of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space Enemy Pawn", Meta = (AllowPrivateAccess = "true"))
	ASpacecraftPawn* Target;

	/** Radius of the detection area (sphere) of this spacecraft. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Pawn", Meta = (AllowPrivateAccess = "true"))
	float DetectionAreaRadius;

	/** Radius of the close proximity area (sphere) of this spacecraft. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Pawn", Meta = (AllowPrivateAccess = "true"))
	float CloseQuartersAreaRadius;

	/**
	 * Whenever this spacecraft detects an enemy, along with the start of moving towards it,
	 * it will also increase the radius of its detection area by this variable's value per cent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Pawn", Meta = (AllowPrivateAccess = "true"))
	float DetectionAreaRadiusModifier;

	/**
	 * Whenever this spacecraft's enemy gets close enough, along with sitting in place and attacking,
	 * it will also increase the radius of its close quarters area by this variable's value per cent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Space Enemy Pawn", Meta = (AllowPrivateAccess = "true"))
	float CloseQuartersAreaRadiusModifier;

	/** List of loot chests and their respective chances of getting spawned during this spacecraft's destruction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	TArray<FLootChestWithChance_KeyValue_Pair> LootChestsAndChances;

	/** Bounding box inside which loot chests will be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	FBox LootBoundingBox;

public:
	/** Sets default values. */
	ASpaceEnemyPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called when an object enters this spacecraft's detection area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Space Enemy Pawn", Meta = (DisplayName = "OnObjectEnterDetectionArea"))
	void ExecuteOnObjectEnterDetectionArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void ExecuteOnObjectEnterDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Called when an object exits this spacecraft's detection area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Space Enemy Pawn", Meta = (DisplayName = "OnObjectExitDetectionArea"))
	void ExecuteOnObjectExitDetectionArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void ExecuteOnObjectExitDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called when an object enters this spacecraft's close proximity area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Space Enemy Pawn", Meta = (DisplayName = "OnObjectEnterCloseQuartersArea"))
	void ExecuteOnObjectEnterCloseQuartersArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void ExecuteOnObjectEnterCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Called when an object exits this spacecraft's close proximity area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Space Enemy Pawn", Meta = (DisplayName = "OnObjectExitCloseQuartersArea"))
	void ExecuteOnObjectExitCloseQuartersArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void ExecuteOnObjectExitCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

protected:

	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

protected:
	/**
	 * Called right before the spacecraft is removed from the world.
	 * 
	 * @param bShouldPlayDestroyEffects [ref] if set to true, an explosion will be spawned at the spacecraft's location.
	 * @param bShouldBeDestroyedForGood [ref] if set to false, the attempt to remove this actor is canceled.
	 */
	virtual void PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed) override;
	

	/**********************************
			 WEAPONS INTERFACE
	**********************************/

public:

	/** Activates primary weapons on the spacecraft. */
	virtual void BeginFiringWeapon() override;

	/** Deactivates primary weapons on the spacecraft. */
	virtual void EndFiringWeapon() override;


	/**********************************
				  LOOT
	**********************************/

private:
	void TryToCreateLootChests();
	void SpawnLootChest(TSubclassOf<ALootChest> ChestTypeToSpawn);


	/**********************************
				GETTERS
	**********************************/

public:

};

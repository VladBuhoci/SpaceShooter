// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Pawns/SpacecraftPawn.h"
#include "Listeners/MousePointerListener.h"
#include "Loot/Creation/WeaponBlueprint.h"

#include "CoreMinimal.h"
#include "SpaceEnemyPawn.generated.h"

// Forward declarations.
class USphereComponent;
class UWidgetComponent;

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
class SPACESHOOTER_API ASpaceEnemyPawn : public ASpacecraftPawn, public IMousePointerListener
{
	GENERATED_BODY()
	
private:
	/* Used to detect whenever a Spacecraft pawn enters or exits this spacecraft's "field of view". */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USphereComponent* DetectionArea;

	/* Used to detect whenever a Spacecraft pawn enters or exits this spacecraft's close quarters area. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	USphereComponent* CloseQuartersArea;

	/** Widget component presenting info such as name, hit/shield points in a simple widget. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | Components", Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* SurvivabilitySummaryWidgetComponent;

	/** The targeted spacecraft of this pawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	ASpacecraftPawn* Target;

	/**
	 * If set to true, this NPC will not stop hunting down its enemies until there are none
	 *		left in the level, starting with the closest ones.
	 * If set to true, the detection area's radius is permanently set to the modified value,
	 *		using DetectionAreaRadiusModifier to calculate it.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	bool bAlwaysAggressive;

	/** Radius of the detection area (sphere) of this spacecraft. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	float DetectionAreaRadius;

	/** Radius of the close proximity area (sphere) of this spacecraft. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	float CloseQuartersAreaRadius;

	/**
	 * Whenever this spacecraft detects an enemy, along with the start of moving towards it,
	 * it will also increase the radius of its detection area by this variable's value per cent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	float DetectionAreaRadiusModifier;

	/**
	 * Whenever this spacecraft's enemy gets close enough, along with sitting in place and attacking,
	 * it will also increase the radius of its close quarters area by this variable's value per cent.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC", Meta = (AllowPrivateAccess = "true"))
	float CloseQuartersAreaRadiusModifier;

public:
	/** Sets default values. */
	ASpaceEnemyPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called when an object enters this spacecraft's detection area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spacecraft | NPC", Meta = (DisplayName = "OnObjectEnterDetectionArea"))
	void ExecuteOnObjectEnterDetectionArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void ExecuteOnObjectEnterDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Called when an object exits this spacecraft's detection area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spacecraft | NPC", Meta = (DisplayName = "OnObjectExitDetectionArea"))
	void ExecuteOnObjectExitDetectionArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void ExecuteOnObjectExitDetectionArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Called when an object enters this spacecraft's close proximity area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spacecraft | NPC", Meta = (DisplayName = "OnObjectEnterCloseQuartersArea"))
	void ExecuteOnObjectEnterCloseQuartersArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void ExecuteOnObjectEnterCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Called when an object exits this spacecraft's close proximity area. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spacecraft | NPC", Meta = (DisplayName = "OnObjectExitCloseQuartersArea"))
	void ExecuteOnObjectExitCloseQuartersArea               (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void ExecuteOnObjectExitCloseQuartersArea_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

protected:

	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;

private:
	/** Will return OriginalRadius + OriginalRadius * RadiusModifier % 100. */
	float ComputeModifiedAreaRadius(float OriginalRadius, float RadiusModifier);


	/**********************************
		 MOUSE LISTENER INTERFACE
	**********************************/

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spacecraft | NPC | Mouse Listener Interface")
	void OnMouseEnter();
	virtual void OnMouseEnter_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Spacecraft | NPC | Mouse Listener Interface")
	void OnMouseLeave();
	virtual void OnMouseLeave_Implementation() override;

protected:
	/** True if the mouse cursor is located over this object at the moment. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacecraft | NPC | Mouse Listener Interface")
	bool bCurrentlyPointedAt;


	/**********************************
		  SURVIVABILITY INTERFACE
	**********************************/

protected:
	/**
	 * Called after taking damage, a value not great enough to be destroyed yet.
	 * Damage has already been applied and checks done at the time of this method being called.
	 */
	virtual void OnDamageTaken(ASpacecraftPawn* DamageCauser, int32 DamageTaken) override;

	/**
	 * Called right before the spacecraft is removed from the world.
	 * 
	 * @param bShouldPlayDestroyEffects [ref] if set to true, an explosion will be spawned at the spacecraft's location.
	 * @param bShouldBeDestroyedForGood [ref] if set to false, the attempt to remove this actor is canceled.
	 */
	virtual void PreDestroy(bool & bShouldPlayDestroyEffects, bool & bShouldBeDestroyed) override;

private:
	void OnNewEnemyFound(ASpacecraftPawn* NewTarget);
	void OnEnemyLost();
	void OnEnemyEnterCombatArea();
	void OnEnemyExitCombatArea();

	void UpdateStateBasedOnAggressiveness();

private:
	/** Used to control the timer that takes care of the tiny HP/SP HUD visibility after taking damage. */
	FTimerHandle ShowSurvivabilityWidgetOnDamageTakenTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | NPC | Misc", Meta = (AllowPrivateAccess = "true"))
	float SurvivabilityWidgetVisibilityTime;
	

	/**********************************
			 WEAPONS INTERFACE
	**********************************/

public:
	/** Activates equipped weapon on the spacecraft. */
	virtual void BeginFiringWeapon() override;

	/** Deactivates equipped weapon on the spacecraft. */
	virtual void EndFiringWeapon() override;


	/**********************************
	  WEAPON STATE LISTENER INTERFACE
	**********************************/

public:
	virtual void OnWeaponOverheated_Implementation(AWeapon* OverheatedWeapon) override;
	virtual void OnWeaponCooledDown_Implementation(AWeapon* CooledDownWeapon) override;

private:
	bool bWaitingForWeaponToCoolDown;


	/**********************************
			  LOOT INTERFACE
	**********************************/

private:
	/** List of loot chests and their respective chances of getting spawned during this spacecraft's destruction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	TArray<FLootChestWithChance_KeyValue_Pair> LootChestsAndChances;

	/** Bounding box inside which loot chests will be spawned. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spacecraft | Loot", Meta = (AllowPrivateAccess = "true"))
	FBox LootBoundingBox;

private:
	void TryToCreateLootChests();
	void SpawnLootChest(TSubclassOf<ALootChest> ChestTypeToSpawn);


	/**********************************
				 GETTERS
	**********************************/

public:
	bool IsAlwaysAggressive() const { return bAlwaysAggressive; }
	TArray<ASpacecraftPawn*> GetAllSpacecraftsInDetectionArea(ESpacecraftFaction Factions = ESpacecraftFaction::Both);


	/**********************************
				 SETTERS
	**********************************/

public:
	void SetIsAlwaysAggressive(bool bState) { bAlwaysAggressive = bState; UpdateStateBasedOnAggressiveness(); }


	/**********************************
				  UTILS
	**********************************/

private:
	float GetDistanceToCurrentTarget();
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpacecraftPawn.h"
#include "SpaceEnemyPawn.generated.h"

// Forward declarations.
class USphereComponent;


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
	virtual void DestroySpacecraft();
	

	/**********************************
			 WEAPONS INTERFACE
	**********************************/

public:

	/** Activates primary weapons on the spacecraft. */
	virtual void BeginFiringPrimaryWeapons() override;

	/** Deactivates primary weapons on the spacecraft. */
	virtual void EndFiringPrimaryWeapons() override;


	/**********************************
				GETTERS
	**********************************/

public:

};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "SpacecraftPawn.h"
#include "SpaceEnemyPawn.generated.h"

/**
 * Base class of all Enemy spacecraft entities.
 */
UCLASS()
class SPACESHOOTER_API ASpaceEnemyPawn : public ASpacecraftPawn
{
	GENERATED_BODY()
	
private:


public:
	/** Sets default values. */
	ASpaceEnemyPawn();

	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;


	/**********************************
			 MOVEMENT INTERFACE
	**********************************/

protected:

	virtual void OnTurboModeActivated() override;
	virtual void OnTurboModeDeactivated() override;
	

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

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LootChest.generated.h"

// Forward declarations.
class UPhysicsConstraintComponent;


/**
 * Base class for loot chests.
 */
UCLASS()
class SPACESHOOTER_API ALootChest : public AActor
{
	GENERATED_BODY()

	/** Mesh of this chest. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ChestMesh;

	/** Physical constraint component for this loot chest. Primarily used to keep it in the XY plane. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Loot Chest", Meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* PhysicsConstraintComponent;
	
public:	
	/** Sets default values. */
	ALootChest();

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

public:	
	/** Called every frame. */
	virtual void Tick(float DeltaTime) override;
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "XYOnlyPhysicsConstraintComponent.generated.h"


/**
 * Component that locks the actor which is using it on the XY plane (both position and rotation transformations).
 */
UCLASS()
class SPACESHOOTER_API UXYOnlyPhysicsConstraintComponent : public UPhysicsConstraintComponent
{
	GENERATED_BODY()
	
public:
	/** Sets default values. */
	UXYOnlyPhysicsConstraintComponent();
	
	void SetActorConstrainedComponent(USceneComponent* ConstrainedComp);
};

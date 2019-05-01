// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "XYOnlyPhysicsConstraintComponent.h"

#include "Components/SceneComponent.h"


UXYOnlyPhysicsConstraintComponent::UXYOnlyPhysicsConstraintComponent()
{
	SetLinearXLimit(LCM_Free, 0.0f);			// Movement on X
	SetLinearYLimit(LCM_Free, 0.0f);			// Movement on Y
	SetLinearZLimit(LCM_Locked, 0.0f);			// Movement on Z
	SetAngularTwistLimit(ACM_Locked, 0.0f);		// Rotation around X
	SetAngularSwing2Limit(ACM_Locked, 0.0f);	// Rotation around Y
	SetAngularSwing1Limit(ACM_Free, 0.0f);		// Rotation around Z
}

void UXYOnlyPhysicsConstraintComponent::SetActorConstrainedComponent(USceneComponent* ConstrainedComp)
{
	if (ConstrainedComp != NULL)
		ComponentName1.ComponentName = ConstrainedComp->GetFName();
	
	// Leave ComponentName2 NULL to constrain this actor to the world.
}

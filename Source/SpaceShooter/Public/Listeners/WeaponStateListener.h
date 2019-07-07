// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "Runtime/CoreUObject/Public/UObject/Interface.h"
#include "WeaponStateListener.generated.h"

// Forward declarations.
class AWeapon;


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponStateListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Listens to weapon state changes (overheat, cool down).
 */
class SPACESHOOTER_API IWeaponStateListener
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/** Called when the weapon has overheated. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon State Listener")
	void OnWeaponOverheated(AWeapon* OverheatedWeapon);

	/** Called when the weapon has cooled down and is ready to be used again. */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon State Listener")
	void OnWeaponCooledDown(AWeapon* CooledDownWeapon);
};

// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "Listeners/GoalListener.h"

#include "GameModes/Gameplay/GameplaySpecification.h"
#include "SubclassOf.h"
#include "GoalDescription.generated.h"

// Forward declarations.
class UUserWidget;


UENUM(BlueprintType)
enum class EGoalUpdateType : uint8
{
	NoUpdate,
	EnemyDestroyed
};

UCLASS(BlueprintType)
class SPACESHOOTER_API UGoalDescription : public UGameplaySpecification
{
	GENERATED_BODY()

private:
	TArray<IGoalListener*> GoalListeners;

	TSubclassOf<UUserWidget> GoalWidgetClass;

public:
	void RegisterListener(IGoalListener* NewListener);
	void UnregisterListener(IGoalListener* ListenerToRemove);

	virtual void UpdateGoal(EGoalUpdateType UpdateType) {}

protected:
	void NotifyGoalListeners();


	/**********************************
				 GETTERS
	**********************************/

public:
	TSubclassOf<UUserWidget> GetWidgetClass() const { return GoalWidgetClass; }


	/**********************************
				 SETTERS
	**********************************/

public:
	void SetWidgetClass(TSubclassOf<UUserWidget> WidgetClass) { GoalWidgetClass = WidgetClass; }
};

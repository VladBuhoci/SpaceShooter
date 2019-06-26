// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "UI/SpaceHUD.h"
#include "CampaignHUD.generated.h"


/**
 * HUD class for the Campaign game mode.
 */
UCLASS()
class SPACESHOOTER_API ACampaignHUD : public ASpaceHUD
{
	GENERATED_BODY()
	
	/** Type of in-game chapter statistics menu widget to spawn when completing a chapter or when destroyed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Space HUD | Campaign HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ChapterEndStatsMenuWidgetType;

	/** Type of in-game chapter goal widget to spawn at level start. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD | Campaign HUD", Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ChapterGoalWidgetType;

	/** Pointer to the in-game chapter statistics menu widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD | Campaign HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* ChapterEndStatsMenuWidget;

	/** Pointer to the in-game chapter goal widget object. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Space HUD | Campaign HUD", Meta = (AllowPrivateAccess = "true"))
	UUserWidget* GoalWidget;

public:
	/** Sets default values. */
	ACampaignHUD();

protected:
	virtual void CreateAndAddWidgets() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Space HUD | Campaign HUD")
	void ToggleChapterEndStatsMenuInterface();

	void SetGoalWidget(TSubclassOf<UUserWidget> GoalWidgetClass);
};

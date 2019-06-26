// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "UI/CampaignHUD.h"
#include "Controllers/SpacePlayerController.h"

#include "UserWidget.h"


ACampaignHUD::ACampaignHUD()
{

}

void ACampaignHUD::CreateAndAddWidgets()
{
	Super::CreateAndAddWidgets();

	TryCreateAndAddWidget(ChapterEndStatsMenuWidgetType, ChapterEndStatsMenuWidget, ESlateVisibility::Collapsed);
	TryCreateAndAddWidget(ChapterGoalWidgetType, GoalWidget, ESlateVisibility::Visible);

	if (GoalWidget)
	{
		GoalWidget->SetAnchorsInViewport(FAnchors(100.0f, 50.0f));	// Anchor set in the middle of the left side.
		GoalWidget->SetPositionInViewport(FVector2D(80.0f, 0.0f));
		GoalWidget->SetDesiredSizeInViewport(FVector2D(80.0f, 40.0f));
	}
}

void ACampaignHUD::ToggleChapterEndStatsMenuInterface()
{
	if (ChapterEndStatsMenuWidget)
	{
		ToggleLevelEndStatsMenuInterface(ChapterEndStatsMenuWidget);
	}
}

void ACampaignHUD::SetGoalWidget(TSubclassOf<UUserWidget> GoalWidgetClass)
{
	ChapterGoalWidgetType = GoalWidgetClass;
}

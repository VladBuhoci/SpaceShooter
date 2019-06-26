// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "UI/CampaignHUD.h"
#include "Controllers/SpacePlayerController.h"

#include "UserWidget.h"


ACampaignHUD::ACampaignHUD()
{

}

void ACampaignHUD::CreateAndAddWidgets(TArray<UUserWidget*> & GameTimeVisibleWidgets)
{
	Super::CreateAndAddWidgets(GameTimeVisibleWidgets);

	TryCreateAndAddWidget(ChapterEndStatsMenuWidgetType, ChapterEndStatsMenuWidget, ESlateVisibility::Collapsed);
	TryCreateAndAddWidget(ChapterGoalWidgetType, GoalWidget, ESlateVisibility::Visible);

	GameTimeVisibleWidgets.Add(GoalWidget);
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

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

	if (ChapterEndStatsMenuWidgetType)
	{
		ChapterEndStatsMenuWidget = CreateWidget<UUserWidget>(GetSpacePlayerController(), ChapterEndStatsMenuWidgetType);
		if (ChapterEndStatsMenuWidget)
		{
			ChapterEndStatsMenuWidget->AddToViewport(102);
			ChapterEndStatsMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ACampaignHUD::ToggleChapterEndStatsMenuInterface()
{
	if (ChapterEndStatsMenuWidget)
	{
		ToggleLevelEndStatsMenuInterface(ChapterEndStatsMenuWidget);
	}
}

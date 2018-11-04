// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceHUD.h"

#include "ConstructorHelpers.h"

#include "Materials/MaterialInterface.h"


ASpaceHUD::ASpaceHUD()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> targetImageFinder(TEXT("Material'/Game/Materials/Crosshairs/Crosshair_1.Crosshair_1'"));
	
	if (targetImageFinder.Succeeded())
	{
		TargetIcon = targetImageFinder.Object;
	}

	TargetPosX     = 0.0f;
	TargetPosY     = 0.0f;
	bCanDrawTarget = true;
}

void ASpaceHUD::DrawHUD()
{
	Super::DrawHUD();

	if (bCanDrawTarget)
	{
		if (TargetIcon)
		{
			// TODO: Is there any way to determine the width and height programmatically?
			DrawMaterialSimple(TargetIcon, TargetPosX - 16, TargetPosY - 16, 32, 32);
		}
	}
	else
	{

	}
}

void ASpaceHUD::UpdateTargetIconPosition(float newPosX, float newPosY)
{
	TargetPosX = newPosX;
	TargetPosY = newPosY;
}

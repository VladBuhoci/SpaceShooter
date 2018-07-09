// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceHUD.h"

#include "ConstructorHelpers.h"

#include "Materials/MaterialInterface.h"


ASpaceHUD::ASpaceHUD()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> targetImageFinder (TEXT("Material'/Game/Materials/Crosshairs/Crosshair_1.Crosshair_1'"));
	
	if (targetImageFinder.Succeeded())
	{
		TargetImage = targetImageFinder.Object;
	}

	TargetPosX = TargetPosY = 0.0f;
}

void ASpaceHUD::DrawHUD()
{
	Super::DrawHUD();

	if (TargetImage)
	{
		// Is there any way to determine the width and height programatically?
		DrawMaterialSimple(TargetImage, TargetPosX, TargetPosY, 32, 32);
	}
}

void ASpaceHUD::UpdateTargetImagePosition(float newPosX, float newPosY)
{
	TargetPosX = newPosX;
	TargetPosY = newPosY;
}

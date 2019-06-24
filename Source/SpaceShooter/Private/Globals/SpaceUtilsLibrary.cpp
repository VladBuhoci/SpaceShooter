// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceUtilsLibrary.h"
#include "SpacePlayerPawn.h"

#include "Kismet/GameplayStatics.h"


FText USpaceUtilsLibrary::GetFloatAsText(float Number, int32 Precision, bool IncludeLeadingZero)
{
    // Round to integral if having something like 1.9999 within precision
	float Rounded = roundf(Number);

	if (FMath::Abs(Number - Rounded) < FMath::Pow(10, - Precision))
	{       
		Number = Rounded;
	}

	FNumberFormattingOptions NumberFormat;

	NumberFormat.MinimumIntegralDigits   = (IncludeLeadingZero) ? 1 : 0;
	NumberFormat.MaximumIntegralDigits   = 10000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;

	return FText::AsNumber(Number, & NumberFormat); 
}

bool USpaceUtilsLibrary::IsPlayerAlive(const UObject* WorldContextObject)
{
	ASpacePlayerPawn* SpacePlayerPawn = Cast<ASpacePlayerPawn>(UGameplayStatics::GetPlayerPawn(WorldContextObject, 0));

	return SpacePlayerPawn ? SpacePlayerPawn->IsNotDestroyed() : false;
}

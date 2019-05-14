// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacecraftTurboModeCameraShake.h"


USpacecraftTurboModeCameraShake::USpacecraftTurboModeCameraShake()
{
	OscillationDuration            = 0.20f;
	
	RotOscillation.Roll.Amplitude  = 2.0f;
	RotOscillation.Roll.Frequency  = 8.0f;

	RotOscillation.Pitch.Amplitude = 2.0f;
	RotOscillation.Pitch.Frequency = 7.5f;

	RotOscillation.Yaw.Amplitude   = 2.0f;
	RotOscillation.Yaw.Frequency   = 8.0f;

	LocOscillation.X.Amplitude     = 3.0f;
	LocOscillation.X.Frequency     = 15.0f;

	LocOscillation.Y.Amplitude     = 3.0f;
	LocOscillation.Y.Frequency     = 15.0f;

	LocOscillation.Z.Amplitude     = 5.0f;
	LocOscillation.Z.Frequency     = 15.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
}

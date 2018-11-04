// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpacecraftDestructionCameraShake.h"


USpacecraftDestructionCameraShake::USpacecraftDestructionCameraShake()
{
	OscillationDuration            = 0.75f;
	
	RotOscillation.Roll.Amplitude  = 3.0f;
	RotOscillation.Roll.Frequency  = 50.0f;
	
	RotOscillation.Pitch.Amplitude = 3.0f;
	RotOscillation.Pitch.Frequency = 50.0f;

	LocOscillation.X.Amplitude     = 20.0f;
	LocOscillation.X.Frequency     = 100.0f;

	LocOscillation.Y.Amplitude     = 20.0f;
	LocOscillation.Y.Frequency     = 100.0f;

	LocOscillation.Z.Amplitude     = 100.0f;
	LocOscillation.Z.Frequency     = 30.0f;
	LocOscillation.Z.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
}

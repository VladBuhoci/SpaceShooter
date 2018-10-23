// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#include "SpaceEnemyPawn.h"
#include "SpaceEnemyController.h"


ASpaceEnemyPawn::ASpaceEnemyPawn()
{
	AIControllerClass = ASpaceEnemyController::StaticClass();
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI     = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASpaceEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

}

void ASpaceEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpaceEnemyPawn::OnTurboModeActivated()
{

}

void ASpaceEnemyPawn::OnTurboModeDeactivated()
{

}

void ASpaceEnemyPawn::BeginFiringPrimaryWeapons()
{
	Super::BeginFiringPrimaryWeapons();

}

void ASpaceEnemyPawn::EndFiringPrimaryWeapons()
{
	Super::EndFiringPrimaryWeapons();

}

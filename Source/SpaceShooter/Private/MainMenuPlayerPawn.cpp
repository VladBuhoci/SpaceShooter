// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenuPlayerPawn.h"


// Sets default values
AMainMenuPlayerPawn::AMainMenuPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainMenuPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainMenuPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainMenuPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


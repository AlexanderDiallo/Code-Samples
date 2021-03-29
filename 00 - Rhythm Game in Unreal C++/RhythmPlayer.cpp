// Fill out your copyright notice in the Description page of Project Settings.


#include "RhythmPlayer.h"

// Sets default values
ARhythmPlayer::ARhythmPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARhythmPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARhythmPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARhythmPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


// Fill out your copyright notice in the Description page of Project Settings.


#include "NoteLane.h"

#include "Engine/World.h"
#include "Public/BongoBeaterGameMode.h"
#include "Components/SplineComponent.h"
#include "Public/SongDataStruct.h"

// Sets default values
ANoteLane::ANoteLane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootSceneComponent");
    SetRootComponent(RootSceneComponent);

    SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
    FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepWorld, false);
    SplineComponent->SetupAttachment(RootSceneComponent);
}

// Called when the game starts or when spawned
void ANoteLane::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANoteLane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ANoteLane::GetLocationOnSpline(float Progression) const
{
    Progression = FMath::Clamp<float>(Progression, 0.0f, 1.0f);
    Progression = 1.f - Progression;

    return SplineComponent->GetLocationAtTime(
        Progression,
        ESplineCoordinateSpace::World
    );
}

float ANoteLane::GetNoteHitLocationOnSpline() const
{
    return NoteHitLocationOnSpline;
}

float ANoteLane::GetEarliestNoteHitLocationOnSpline() const
{
    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode) { return 0.0f; }

    const FSongData* CurrentSongData = GameMode->GetCurrentSongData();
    if (!CurrentSongData) { return 0.0f; }

    const float BeatsShownInAdvance = GameMode->GetBeatsShownInAdvance();

    const float LocationOnSpline = (GameMode->GetBeatsShownInAdvance() + CurrentSongData->EarliestRegisteredHitDeltaInBeats) / 
        GameMode->GetBeatsShownInAdvance() * 
        NoteHitLocationOnSpline
        ;

    return LocationOnSpline;
}

float ANoteLane::GetLatestNoteHitLocationOnSpline() const
{
    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode) { return 1.0f; }

    const FSongData* CurrentSongData = GameMode->GetCurrentSongData();
    if (!CurrentSongData) { return 1.0f; }

    const float BeatsShownInAdvance = GameMode->GetBeatsShownInAdvance();

    const float LocationOnSpline = (GameMode->GetBeatsShownInAdvance() + CurrentSongData->LatestRegisteredHitDeltaInBeats) /
        GameMode->GetBeatsShownInAdvance() *
        NoteHitLocationOnSpline
        ;

    return LocationOnSpline;
}

TSubclassOf<AAsteroid> ANoteLane::GetAsteroidBlueprint() const
{
    return AsteroidBlueprint;
}


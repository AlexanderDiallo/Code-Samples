// Fill out your copyright notice in the Description page of Project Settings.


#include "Asteroid.h"

#include "Engine/World.h"
#include "Public/BongoBeaterGameMode.h"
#include "Public/AsteroidSpawner.h"
#include "Public/NoteLane.h"

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UpdateLocationBasedOnBeat();

}

void AAsteroid::SetNoteMissed(bool bInNoteMissed)
{
    NoteData.bNoteMissed = bInNoteMissed;
}

void AAsteroid::SetAsteroidSpawnerReference(AAsteroidSpawner* AsteroidSpawnerReference)
{
    AsteroidSpawner = AsteroidSpawnerReference;
}

void AAsteroid::SetLaneReference(const ANoteLane* LaneReference)
{
    Lane = LaneReference;

    /*if (Lane)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: spawned at lane %s"), *GetName(), *(Lane->GetName()));
    }*/
}

void AAsteroid::SetNoteData(const FNoteData& NoteDataReference)
{
    //TODO: create a copy constructor for FNoteData

    NoteData = NoteDataReference;

    /*NoteData.Beat = NoteDataToCopy.Beat;
    NoteData.LaneIndex = NoteDataToCopy.LaneIndex;*/
}

const FNoteData& AAsteroid::GetNoteData() const
{
    return NoteData;
}

int32 AAsteroid::GetLaneIndex() const
{
    return NoteData.LaneIndex;
}

void AAsteroid::UpdateLocationBasedOnBeat()
{
    if (!Lane) { return; }

    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode) { return; }

    const float BeatsShownInAdvance = GameMode->GetBeatsShownInAdvance();
    const float SongPositionInBeats = GameMode->GetSongPositionInBeats();

    const float MovementProgression = (BeatsShownInAdvance -
        (NoteData.Beat - SongPositionInBeats)) /
        BeatsShownInAdvance *
        Lane->GetNoteHitLocationOnSpline()
        ;

    /*UE_LOG(LogTemp, Error, TEXT("%s: BeatsInAdvance = %f, BeatToHit = %f, SongPositionInBeats = %f, Result = %f"), 
        *GetName(),
        GameMode->GetBeatsShownInAdvance(),
        NoteData.Beat,
        GameMode->GetSongPositionInBeats(),
        MovementProgression
    );*/

    //if asteroid has reached end of spline, destroy self
    if (MovementProgression >= 1.0f)
    {
        //TODO: fire Asteroid/Note destroy event

        if (AsteroidSpawner)
        {
            AsteroidSpawner->RemoveAsteroidReference(this);

            if (!NoteData.bNoteMissed)
            {
                NoteData.bNoteMissed = true;

                FNoteEventData NoteEventData;
                NoteEventData.NoteHit = false;

                AsteroidSpawner->OnNoteEvent.Broadcast(NoteEventData);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("%s: AsteroidSpawner reference has not been set"), *GetName());
        }

        Destroy();
    }



    SetActorLocation(
        Lane->GetLocationOnSpline(MovementProgression)
    );
}


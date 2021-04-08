// Fill out your copyright notice in the Description page of Project Settings.


#include "AsteroidSpawner.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Math/TransformNonVectorized.h"

#include "Public/BongoBeaterGameMode.h"
#include "Public/Asteroid.h"
#include "Public/SongDataStruct.h"
#include "Public/NoteLane.h"

// Sets default values
AAsteroidSpawner::AAsteroidSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();

    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to find BongoBeaterGameMode"), *GetName());
        return;
    }

    GameMode->OnSpawnNote.AddDynamic(this, &AAsteroidSpawner::SpawnAsteroid);
    GameMode->OnAttemptedNoteHit.AddDynamic(this, &AAsteroidSpawner::HandleAttemptedNoteHit);
    OnNoteEvent.AddDynamic(GameMode, &ABongoBeaterGameMode::UpdateScore);
    OnNoteEvent.AddDynamic(this, &AAsteroidSpawner::NoteEventTest);


    /*AsteroidsByLaneArray.SetNum(NoteLaneArray.Num());*/
}

// Called every frame
void AAsteroidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    CheckForMissedNotes();
    /*UE_LOG(LogTemp, Error, TEXT("%s: number of asteroids: %i"), *GetName(), AsteroidsByLaneArray[0].AsteroidArray.Num());*/
}

void AAsteroidSpawner::SpawnAsteroid(const FNoteData& EventNoteData)
{
    //ensure values to be passed on to asteroid are valid
    if (EventNoteData.LaneIndex >= LaneDataArray.Num() || EventNoteData.LaneIndex < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to spawn asteroid, lane index %i is out of range (max index is: %i)"), *GetName(), EventNoteData.LaneIndex, LaneDataArray.Num());
        return;
    }

    ANoteLane* NoteLaneOfAsteroid = LaneDataArray[EventNoteData.LaneIndex].NoteLaneReference;

    if (!NoteLaneOfAsteroid)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to spawn asteroid, lane reference at given index is nullptr"), *GetName());
        return;
    }

    //try to spawn the asteroid
    AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>(
        LaneDataArray[EventNoteData.LaneIndex].AsteroidBlueprint,
        FVector(0.f),
        FRotator()
        );

    if (!Asteroid)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Failed to spawn asteroid"), *GetName());
        return;
    }

    //when spawning is successful, set references
    Asteroid->SetLaneReference(NoteLaneOfAsteroid);
    Asteroid->SetNoteData(EventNoteData);
    
    AddAsteroidReference(Asteroid);

    Asteroid->UpdateLocationBasedOnBeat();
}

void AAsteroidSpawner::AddAsteroidReference(AAsteroid* AsteroidReference)
{
    if (!AsteroidReference) 
    { 
        UE_LOG(LogTemp, Error, TEXT("%s: Cannot add asteroid reference of nullptr value"), *GetName());
        return; 
    }

    if (AsteroidReference->GetLaneIndex() >= LaneDataArray.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Cannot add asteroid of an invalid lane index"), *GetName());
        return;
    }

    LaneDataArray[AsteroidReference->GetLaneIndex()].AsteroidArray.AddUnique(AsteroidReference);
    AsteroidReference->SetAsteroidSpawnerReference(this);
}

void AAsteroidSpawner::RemoveAsteroidReference(AAsteroid* AsteroidReference)
{
    if (!AsteroidReference)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Attempted to remove an asteroid reference of nullptr value"), *GetName());
        return;
    }

    LaneDataArray[AsteroidReference->GetLaneIndex()].AsteroidArray.Remove(AsteroidReference);
}

void AAsteroidSpawner::HandleAttemptedNoteHit(int32 LaneIndex)
{
    if (LaneIndex < 0 || LaneIndex >= LaneDataArray.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("%s: HandleAttemptedNoteHit() failed, invalid LaneIndex of %i"), *GetName(), LaneIndex);
        return;
    }

    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode) { return; }
    float SongPositionInBeats = GameMode->GetSongPositionInBeats();

    int32 NumOfAsteroidsInLane = LaneDataArray[LaneIndex].AsteroidArray.Num();
    if (NumOfAsteroidsInLane == 0) { return; }

    UE_LOG(LogTemp, Warning, TEXT("%s: AttemptedNoteHit of lane/bongo index %i at beat %f"), *GetName(), LaneIndex, GameMode->GetSongPositionInBeats());

    if (!GameMode->GetCurrentSongData())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: CurrentSongData of game mode is nullptr"), *GetName());
    }

    /*UE_LOG(LogTemp, Warning, TEXT("%s: Asteroid at beat %f, earliest hit is %f, latest hit is %f"), 
        *GetName(), 
        LaneDataArray[LaneIndex].AsteroidArray[0]->GetNoteData().Beat,
        EarliestPossibleHitInBeats, 
        LatestPossibleHitInBeats);*/

    int32 AsteroidIndex = 0;
    AAsteroid* AsteroidToHit = nullptr;

    float BeatToHitAt = 0.f;
    float EarliestPossibleHitInBeats = 0.f;
    float LatestPossibleHitInBeats = 0.f;
    while (AsteroidIndex < NumOfAsteroidsInLane)
    {
        checkf(LaneDataArray[LaneIndex].AsteroidArray[AsteroidIndex] != nullptr,
            TEXT("%s: A reference to an invalid asteroid found in HandleAttemptedNoteHit()"),
            *GetName()
            );

        BeatToHitAt = LaneDataArray[LaneIndex].AsteroidArray[AsteroidIndex]->GetNoteData().Beat;
        EarliestPossibleHitInBeats = BeatToHitAt + GameMode->GetCurrentSongData()->EarliestRegisteredHitDeltaInBeats;
        LatestPossibleHitInBeats = BeatToHitAt + GameMode->GetCurrentSongData()->LatestRegisteredHitDeltaInBeats;

        if (SongPositionInBeats >= EarliestPossibleHitInBeats &&
            SongPositionInBeats <= LatestPossibleHitInBeats)
        {
            AsteroidToHit = LaneDataArray[LaneIndex].AsteroidArray[AsteroidIndex];
            break;
        }

        AsteroidIndex++;
    }

    if (!AsteroidToHit) { return; }

    UE_LOG(LogTemp, Warning, TEXT("%s: Successful hit of asteroid of beat %f at current beat %f"), 
        *GetName(), 
        AsteroidToHit->GetNoteData().Beat,
        SongPositionInBeats
    );

    FNoteEventData NoteEventData;
    NoteEventData.NoteHit = true;
	NoteEventData.HitPosition = AsteroidToHit->GetTransform().GetLocation();

    if (SongPositionInBeats < BeatToHitAt)
    {
        NoteEventData.HitAccuracy = (1 - ((BeatToHitAt - SongPositionInBeats) / (BeatToHitAt - EarliestPossibleHitInBeats)));
        NoteEventData.HitAccuracy *= -1.0f;
    }
    else
    {
        NoteEventData.HitAccuracy = 1 - ((BeatToHitAt - SongPositionInBeats) / (BeatToHitAt - LatestPossibleHitInBeats));
    }

    OnNoteEvent.Broadcast(NoteEventData);

    RemoveAsteroidReference(AsteroidToHit);
    AsteroidToHit->Destroy();
}

int32 AAsteroidSpawner::GetNumberOfLanes() const
{
    return LaneDataArray.Num();
}

void AAsteroidSpawner::NoteEventTest(FNoteEventData InNoteEventData)
{
    if (InNoteEventData.NoteHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Successful hit of asteroid with %f accuracy"),
            *GetName(),
            InNoteEventData.HitAccuracy
        );
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Note Missed"),
            *GetName()
        );
    }
}

void AAsteroidSpawner::CheckForMissedNotes()
{
    ABongoBeaterGameMode* GameMode = GetWorld()->GetAuthGameMode<ABongoBeaterGameMode>();
    if (!GameMode) { return; }
    float SongPositionInBeats = GameMode->GetSongPositionInBeats();

    for (const auto& LaneData : LaneDataArray)
    {
        for (const auto& Asteroid : LaneData.AsteroidArray)
        {
            checkf(Asteroid,
                TEXT("%s: A reference to an invalid asteroid found in CheckForMissedNotes()"),
                *GetName()
            );

            float BeatToHitAt = Asteroid->GetNoteData().Beat;
            float LatestPossibleHitInBeats = BeatToHitAt + GameMode->GetCurrentSongData()->LatestRegisteredHitDeltaInBeats;

            if (SongPositionInBeats > LatestPossibleHitInBeats)
            {
                if (!Asteroid->GetNoteData().bNoteMissed)
                {
                    Asteroid->SetNoteMissed(true);

                    FNoteEventData NoteEventData;
                    NoteEventData.NoteHit = false;

                    OnNoteEvent.Broadcast(NoteEventData);
                }
            }
        }
    }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BongoBeaterGameMode.h"

#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Public/DataReceiverBFL.h"
#include "Public/AsteroidSpawner.h"

#include "Components/StaticMeshComponent.h"

ABongoBeaterGameMode::ABongoBeaterGameMode()
{
    SynthSamplePlayerComponent = CreateDefaultSubobject<USynthSamplePlayer>("SynthSamplePlayerComponent");

    SetRootComponent(SynthSamplePlayerComponent);
}

void ABongoBeaterGameMode::BeginPlay()
{
    Super::BeginPlay();

    PlaySongOfIndex(0);

}

void ABongoBeaterGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /*UE_LOG(LogTemp, Error, TEXT("SongPositionInSeconds: %f, SongPositionInBeats: %f, SecondsPerBeat: %f, BPM: %f"),
        SongPositionInSeconds,
        SongPositionInBeats,
        SecondsPerBeat,
        CurrentSongData->SongBPM
    );*/

    /*if (NextNoteIndex < NoteData.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("%f is waiting for %f"), NoteData[NextNoteIndex].Beat, (SongPositionInBeats - BeatsShownInAdvance));
    }*/

    if (!CurrentSongData) { return; }

    while (NextNoteIndex < NoteData.Num() &&
        NoteData[NextNoteIndex].Beat - CurrentSongData->BeatsShownInAdvance < GetSongPositionInBeats())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: Note at beat %f to be spawned on lane %i at current beat %f"),
            *GetName(),
            NoteData[NextNoteIndex].Beat,
            NoteData[NextNoteIndex].LaneIndex,
            GetSongPositionInBeats()
        );

        OnSpawnNote.Broadcast(NoteData[NextNoteIndex]);

        NextNoteIndex++;
    }

    UpdateCurrentBeat();
}

void ABongoBeaterGameMode::PlaySongOfIndex(int32 SongIndex)
{
    NoteData.Empty();

    if (!SongDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: No general wave data table has been assigned"), *GetName());
        return;
    }

    SetCurrentSongData(SongIndex);
    if (!CurrentSongData) { return; }

    SynthSamplePlayerComponent->SetSoundWave(CurrentSongData->Song);
    /*SynthSamplePlayerComponent->Start();
    SkipToPlayBackTime(CurrentSongData->SongPlaybackStartTime);*/

    bIsPlaying = true;
}

float ABongoBeaterGameMode::GetBeatsShownInAdvance() const
{
    if (!CurrentSongData) { return 0.0f; }
    return CurrentSongData->BeatsShownInAdvance;
}

float ABongoBeaterGameMode::GetSongPositionInBeats() const
{
    if (!CurrentSongData) { return 0.0f; }

    float SongPositionInSeconds = SynthSamplePlayerComponent->GetCurrentPlaybackProgressTime() - CurrentSongData->FirstBeatOffsetInSeconds;
    float SongPositionInBeats = SongPositionInSeconds / SecondsPerBeat;

    return SongPositionInBeats;
}

float ABongoBeaterGameMode::GetEarliestPossibleHitInBeats() const
{
    if (!CurrentSongData) { return 0.0f; }

    return GetSongPositionInBeats() + CurrentSongData->EarliestRegisteredHitDeltaInBeats;;
}

float ABongoBeaterGameMode::GetLatestPossibleHitInBeats() const
{
    if (!CurrentSongData) { return 0.0f; }

    return GetSongPositionInBeats() + CurrentSongData->LatestRegisteredHitDeltaInBeats;
}

const FSongData* ABongoBeaterGameMode::GetCurrentSongData() const
{
    return CurrentSongData;
}

USoundWave* ABongoBeaterGameMode::GetSong() const
{
    if (!CurrentSongData) { return nullptr; }
    return CurrentSongData->Song;
}

void ABongoBeaterGameMode::RaiseAttemptedNoteHitEvent(int32 LaneIndex)
{
    OnAttemptedNoteHit.Broadcast(LaneIndex);
}

float ABongoBeaterGameMode::GetSongBPM() const
{
    if (!CurrentSongData) { return 0.0f; }

    return CurrentSongData->SongBPM;
}

void ABongoBeaterGameMode::SetCurrentSongData(int32 SongIndex)
{
    if (!SongDataTable) { return; }

    TArray<FName> RowNames = SongDataTable->GetRowNames();
    if (RowNames.Num() == 0)
    { 
        UE_LOG(LogTemp, Error, TEXT("%s: Assigned data table is empty"), *GetName());
        return; 
    }
    if (SongIndex < 0 || SongIndex > RowNames.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Invalid song index"), *GetName());
        return;
    }


    CurrentSongData = SongDataTable->FindRow<FSongData>(RowNames[SongIndex], nullptr);

    if (!CurrentSongData)
    {
        UE_LOG(LogTemp, Error, TEXT("%s: Error when loading row data from data table"), *GetName());
        return;
    }

    OnSetCurrentSongData.Broadcast(*CurrentSongData);

    LoadNoteData();

    SecondsPerBeat = 60.f / CurrentSongData->SongBPM;
}

void ABongoBeaterGameMode::SkipToPlayBackTime(float PlaybackTime)
{
    if (!CurrentSongData || !CurrentSongData->Song) { return; }

    PlaybackTime = FMath::Clamp<float>(PlaybackTime, 0.f, CurrentSongData->Song->GetDuration());
    SynthSamplePlayerComponent->SeekToTime(PlaybackTime, ESamplePlayerSeekType::FromBeginning);

    SetNextNoteIndexBasedOnPlaybackTime(PlaybackTime);
}

void ABongoBeaterGameMode::SetNextNoteIndexBasedOnPlaybackTime(float PlaybackTime)
{
    if (!CurrentSongData) { return; }

    float ExpectedSongPositionInBeats = 
        (PlaybackTime - CurrentSongData->FirstBeatOffsetInSeconds) /
        SecondsPerBeat
        ;

    NextNoteIndex = 0;

    while (NextNoteIndex < NoteData.Num() &&
        FMath::Max<float>(NoteData[NextNoteIndex].Beat - CurrentSongData->BeatsShownInAdvance, 0.0f) < ExpectedSongPositionInBeats)
    {
        NextNoteIndex++;
    }

    UE_LOG(LogTemp, Warning, TEXT("%s: Calculated NextNoteIndex is: %i"), *GetName(), NextNoteIndex);
}

void ABongoBeaterGameMode::LoadNoteDataFromLevelSequence()
{
    if (!CurrentSongData) { return; }
    if (!CurrentSongData->LevelSequence)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s: No level sequence for note data has been assigned"), *GetName());
        return;
    }

    NoteData = UDataReceiverBFL::GetSequencerKeys(
        CurrentSongData->SongBPM,
        CurrentSongData->LevelSequence
    );
}

void ABongoBeaterGameMode::UpdateCurrentBeat()
{
    int32 SongProgressionInBeatsTrunc = GetSongPositionInBeats();

    while(SongProgressionInBeatsTrunc > CurrentBeat)
    {
        CurrentBeat++;
        OnBeat.Broadcast();
    }
}

void ABongoBeaterGameMode::UpdateScore(FNoteEventData NoteEventData)
{
    if (NoteEventData.NoteHit)
    {
        ScoreData.CurrentCombo++;
        ScoreData.MaxCombo = FMath::Max<int32>(ScoreData.MaxCombo, ScoreData.CurrentCombo);
        UE_LOG(LogTemp, Warning, TEXT("%s: Current combo is %i"), *GetName(), ScoreData.CurrentCombo);
    }
    else
    {
        ScoreData.MaxCombo = FMath::Max<int32>(ScoreData.MaxCombo, ScoreData.CurrentCombo);
        ScoreData.CurrentCombo = 0;
        UE_LOG(LogTemp, Warning, TEXT("%s: Note missed, max combo is %i"), *GetName(), ScoreData.MaxCombo);
    }
}

const FScoreData& ABongoBeaterGameMode::GetScoreData() const
{
    return ScoreData;
}

void ABongoBeaterGameMode::PlaySong()
{
    if (!CurrentSongData || !CurrentSongData->Song)
    {
        return;
    }

    SynthSamplePlayerComponent->Start();
    SkipToPlayBackTime(CurrentSongData->SongPlaybackStartTime);
}

void ABongoBeaterGameMode::LoadNoteData_Implementation()
{
    //TODO: this is a test dummy implementation, remember to replace with actual implementation

    NoteData.Empty();

    FNoteData DummyData;

    DummyData.Beat = 0.0f;
    DummyData.LaneIndex = 1;
    NoteData.Push(DummyData);

    DummyData.Beat = 4.0f;
    DummyData.LaneIndex = 2;
    NoteData.Push(DummyData);

    DummyData.Beat = 4.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 5.0f;
    DummyData.LaneIndex = 1;
    NoteData.Push(DummyData);

    DummyData.Beat = 6.0f;
    DummyData.LaneIndex = 4;
    NoteData.Push(DummyData);

    DummyData.Beat = 7.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 8.0f;
    DummyData.LaneIndex = 2;
    NoteData.Push(DummyData);

    DummyData.Beat = 8.0f;
    DummyData.LaneIndex = 0;
    NoteData.Push(DummyData);

    DummyData.Beat = 9.0f;
    DummyData.LaneIndex = 4;
    NoteData.Push(DummyData);

    DummyData.Beat = 10.0f;
    DummyData.LaneIndex = 2;
    NoteData.Push(DummyData);

    DummyData.Beat = 11.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 13.0f;
    DummyData.LaneIndex = 4;
    NoteData.Push(DummyData);

    DummyData.Beat = 13.0f;
    DummyData.LaneIndex = 1;
    NoteData.Push(DummyData);

    DummyData.Beat = 14.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 15.0f;
    DummyData.LaneIndex = 0;
    NoteData.Push(DummyData);

    DummyData.Beat = 16.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 17.0f;
    DummyData.LaneIndex = 0;
    NoteData.Push(DummyData);

    DummyData.Beat = 18.0f;
    DummyData.LaneIndex = 4;
    NoteData.Push(DummyData);

    DummyData.Beat = 19.0f;
    DummyData.LaneIndex = 2;
    NoteData.Push(DummyData);

    DummyData.Beat = 20.0f;
    DummyData.LaneIndex = 3;
    NoteData.Push(DummyData);

    DummyData.Beat = 21.0f;
    DummyData.LaneIndex = 1;
    NoteData.Push(DummyData);

    DummyData.Beat = 23.0f;
    DummyData.LaneIndex = 0;
    NoteData.Push(DummyData);

    DummyData.Beat = 23.0f;
    DummyData.LaneIndex = 4;
    NoteData.Push(DummyData);

    DummyData.Beat = 23.0f;
    DummyData.LaneIndex = 2;
    NoteData.Push(DummyData);
}
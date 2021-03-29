// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BongoBeaterGameModeBase.h"

#include "Public/NoteDataStruct.h"
#include "Public/SongDataStruct.h"
#include "Public/ScoreDataStruct.h"
#include "Public/NoteEventDataStruct.h"

#include "BongoBeaterGameMode.generated.h"

class USynthSamplePlayer;
class UDataTable;
class USoundWave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeatDelegate); //TODO: consider having this give current beat as a param
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetCurrentSongDataDelegate, const FSongData&, SongData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnNoteDelegate, const FNoteData&, NoteData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttemptedNoteHitDelegate, int32, LaneIndex);

/**
 * 
 */
UCLASS()
class BONGOBEATER_API ABongoBeaterGameMode : public ABongoBeaterGameModeBase
{
	GENERATED_BODY()
	
public:
    ABongoBeaterGameMode();

protected:
    virtual void BeginPlay() override;

public:

    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void PlaySongOfIndex(int32 SongIndex);

    UFUNCTION(BlueprintCallable)
    void PlaySong();

    UFUNCTION(BlueprintCallable)
    float GetBeatsShownInAdvance() const;

    UFUNCTION(BlueprintCallable)
    float GetSongPositionInBeats() const;

    UFUNCTION(BlueprintCallable)
    void BeginSoundVisualization(TArray<UStaticMeshComponent*> Objects);

    float GetEarliestPossibleHitInBeats() const;
    float GetLatestPossibleHitInBeats() const;

    const FSongData* GetCurrentSongData() const;

	UFUNCTION(BlueprintNativeEvent)
	TArray<float> GetAmplitude(USoundWave* SoundWave, int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets);

	TArray<float> GetAmplitude_Implementation(USoundWave* SoundWave, int32 Channel, float StartTime, float TimeLength, int32 AmplitudeBuckets);

    UFUNCTION(BlueprintCallable)
    USoundWave* GetSong() const;

	UFUNCTION(BlueprintCallable)
	UOnsetNRT* GetOnsetNRT() const;

    UPROPERTY(BlueprintAssignable)
    FBeatDelegate OnBeat;

    UPROPERTY(BlueprintAssignable)
    FSpawnNoteDelegate OnSpawnNote;

    UPROPERTY(BlueprintAssignable)
    FAttemptedNoteHitDelegate OnAttemptedNoteHit;
    UFUNCTION(BlueprintCallable)
    void RaiseAttemptedNoteHitEvent(int32 LaneIndex);

    UPROPERTY(BlueprintAssignable)
    FSetCurrentSongDataDelegate OnSetCurrentSongData;

    UFUNCTION(BlueprintCallable)
    float GetSongBPM() const;

    UFUNCTION()
    void UpdateScore(FNoteEventData NoteEventData);
    UFUNCTION(BlueprintCallable)
    const FScoreData& GetScoreData() const;

protected:

    /**After setting CurrentSongData, this function is used to parse note data and save it to NoteData array*/
    UFUNCTION(BlueprintNativeEvent)
    void LoadNoteData();
    UFUNCTION(BlueprintCallable)
    void LoadNoteDataFromLevelSequence();

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UDataTable* SongDataTable = nullptr;
    
    FSongData* CurrentSongData = nullptr;

    UPROPERTY(BlueprintReadWrite)
    TArray<FNoteData> NoteData;

    /** Required for the playback of the audio*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USynthSamplePlayer* SynthSamplePlayerComponent = nullptr;

    UPROPERTY(EditAnywhere, Category = "Audio")
    class UTimeSynthComponent* TimeSynthComponent;

    UPROPERTY(BlueprintReadOnly)
    float SecondsPerBeat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool UseGeneratedNoteData = false;

    UPROPERTY(BlueprintReadOnly)
    FScoreData ScoreData;

private:
    void SetCurrentSongData(int32 SongIndex = 0);

    void SkipToPlayBackTime(float PlaybackTime);
    void SetNextNoteIndexBasedOnPlaybackTime(float PlaybackTime);

    void UpdateCurrentBeat();

    int32 CurrentBeat = 0;

    int32 NextNoteIndex = 0;

    bool bIsPlaying = false;

    TArray<float> DefaultHeightEnvObjects;
    TArray<UStaticMeshComponent*> EnvironmentObjects;
};

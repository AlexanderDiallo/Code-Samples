#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OnsetNRT.h"
#include "SongDataStruct.generated.h"

// Forward Declaration
class USoundWave;
class ULevelSequence;

/**
 *
 */
USTRUCT(BlueprintType)
struct FSongData : public FTableRowBase
{
    GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		USoundWave* Song = nullptr;

    UPROPERTY(EditAnywhere)
		UOnsetNRT* SongOnsetNRT = nullptr;

    UPROPERTY(EditAnywhere)
        class UTimeSynthClip* TSClip = nullptr;

    UPROPERTY(EditAnywhere)
        ULevelSequence* LevelSequence = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="1.0"))
        float SongBPM = 1.0f;

    /** Defines when (in seconds) the first beat in the audio playback begins*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin="0.0"))
        float FirstBeatOffsetInSeconds = 0.0f;

    /** Defines when (in seconds) the song should start
    This is intended for testing purposes (e.g. designer wants to test a specific section of the song)*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
        float SongPlaybackStartTime = 0.0f;

    /** How early the note should be visually shown relative to when it must be hit */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
        float BeatsShownInAdvance = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMax = "0.0"))
    float EarliestRegisteredHitDeltaInBeats = -0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
    float LatestRegisteredHitDeltaInBeats = 0.2f;
};
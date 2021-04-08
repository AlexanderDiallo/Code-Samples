#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NoteDataStruct.generated.h"

UENUM(BlueprintType)
enum class ENoteType : uint8
{
    Tap 	UMETA(DisplayName = "Tap"),
    Swipe	UMETA(DisplayName = "Swipe"),
    Hold 	UMETA(DisplayName = "Hold")
};

/**
 * Struct used to hold note data such as when and where to spawn it
 */
USTRUCT(BlueprintType)
struct FNoteData : public FTableRowBase
{
    GENERATED_BODY()

public:

    FNoteData& operator=(const FNoteData& NoteData)
    {
        NoteType = NoteData.NoteType;
        LaneIndex = NoteData.LaneIndex;
        Beat = NoteData.Beat;
        BeatEnd = NoteData.Beat;
        return *this;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENoteType NoteType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
    int32 LaneIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
    float Beat = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
    float BeatEnd = 0.0f;

    bool bNoteMissed = false;
};
#pragma once

#include "CoreMinimal.h"

#include "Math/Vector.h"

#include "NoteEventDataStruct.generated.h"

/**
 * Struct used to communicate note hit information whenever the player hits (or attempts to hit) an asteroid/note
 */
USTRUCT(BlueprintType)
struct FNoteEventData
{
    GENERATED_BODY()

public:

    FNoteEventData& operator=(const FNoteEventData& NoteEventData)
    {
        NoteHit = NoteEventData.NoteHit;
        HitAccuracy = NoteEventData.HitAccuracy;
		HitPosition = NoteEventData.HitPosition;
        return *this;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool NoteHit = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HitAccuracy = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector HitPosition = FVector(0.0f, 0.0f, 0.0f);
};

#pragma once

#include "CoreMinimal.h"
#include "ScoreDataStruct.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FScoreData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 CurrentCombo = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 MaxCombo = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Score = 0;
};
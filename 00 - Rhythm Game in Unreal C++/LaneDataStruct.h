#pragma once

#include "CoreMinimal.h"
#include "Public/Asteroid.h"
#include "LaneDataStruct.generated.h"

class ANoteLane;

/**
 * A struct used by NoteLane to group relevant information together
 */
USTRUCT(BlueprintType)
struct FLaneData
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ANoteLane* NoteLaneReference = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AAsteroid> AsteroidBlueprint;

    /** An array of the active asteroids/notes on the note lane*/
    TArray<AAsteroid*>  AsteroidArray;
};
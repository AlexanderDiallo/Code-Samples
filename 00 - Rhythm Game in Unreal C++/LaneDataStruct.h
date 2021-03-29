#pragma once

#include "CoreMinimal.h"
#include "Public/Asteroid.h"
#include "LaneDataStruct.generated.h"

class ANoteLane;

/**
 *
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

    TArray<AAsteroid*>  AsteroidArray;
};
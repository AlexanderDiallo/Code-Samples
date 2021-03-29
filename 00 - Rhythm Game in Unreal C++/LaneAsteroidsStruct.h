#pragma once

#include "CoreMinimal.h"
#include "Public/Asteroid.h"
#include "LaneAsteroidsStruct.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FLaneAsteroids
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<AAsteroid*>  AsteroidArray;
};
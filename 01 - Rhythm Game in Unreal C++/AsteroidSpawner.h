// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/NoteDataStruct.h"
#include "Public/LaneAsteroidsStruct.h"
#include "Public/LaneDataStruct.h"
#include "Public/NoteEventDataStruct.h"
#include "AsteroidSpawner.generated.h"

class ANoteLane;
class AAsteroid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNoteEventDelegate, FNoteEventData, NoteEventData);

UCLASS()
class BONGOBEATER_API AAsteroidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroidSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void SpawnAsteroid(const FNoteData& EventNoteData);

    void AddAsteroidReference(AAsteroid* AsteroidReference);
    void RemoveAsteroidReference(AAsteroid* AsteroidReference);

    UFUNCTION(BlueprintCallable)
    void HandleAttemptedNoteHit(int32 LaneIndex);

    UFUNCTION(BlueprintCallable)
    int32 GetNumberOfLanes() const;

    UPROPERTY(BlueprintAssignable)
    FNoteEventDelegate OnNoteEvent;

protected:

    UFUNCTION()
    void NoteEventTest(FNoteEventData InNoteEventData);

    UPROPERTY(EditAnywhere)
    TArray<FLaneData> LaneDataArray;

private:
    void CheckForMissedNotes();
};

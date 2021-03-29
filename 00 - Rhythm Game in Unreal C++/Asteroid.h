// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Public/NoteDataStruct.h"

#include "Asteroid.generated.h"

class AAsteroidSpawner;
class ANoteLane;
struct FNoteData;

UCLASS()
class BONGOBEATER_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAsteroid();

    void UpdateLocationBasedOnBeat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    
    void SetNoteMissed(bool bInNoteMissed);

    void SetAsteroidSpawnerReference(AAsteroidSpawner* AsteroidSpawnerReference);
    void SetLaneReference(const ANoteLane* LaneReference);
    void SetNoteData(const FNoteData& NoteData);

    const FNoteData& GetNoteData() const;
    int32 GetLaneIndex() const; //TODO: delete, this function is made obsolete by GetNoteData() function

protected:

    AAsteroidSpawner* AsteroidSpawner = nullptr;

    const ANoteLane* Lane = nullptr;

    FNoteData NoteData;

};

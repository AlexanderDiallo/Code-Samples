// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NoteLane.generated.h"

class USceneComponent;
class USplineComponent;
class AAsteroid;

UCLASS()
class BONGOBEATER_API ANoteLane : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANoteLane();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    /** 
    * @param Progression - corresponds to a percentage (normalised float range), with 0.0f end of spline and 1.0f start
    */
    UFUNCTION(BlueprintCallable)
    FVector GetLocationOnSpline(float Progression) const;

    UFUNCTION(BlueprintCallable)
    float GetNoteHitLocationOnSpline() const;

    /** Used to visually represent the earliest note hit location on the spline (e.g. for debug purposes)*/
    UFUNCTION(BlueprintCallable)
    float GetEarliestNoteHitLocationOnSpline() const;

    /** Used to visually represent the latest note hit location on the spline (e.g. for debug purposes)*/
    UFUNCTION(BlueprintCallable)
    float GetLatestNoteHitLocationOnSpline() const;

    TSubclassOf<AAsteroid> GetAsteroidBlueprint() const;

protected:

    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootSceneComponent = nullptr;

    UPROPERTY(VisibleAnywhere)
    USplineComponent* SplineComponent = nullptr;

    /** Allows for different asteroids (i.e. notes) models to be used by each lane, need to set in Blueprint */
    UPROPERTY(EditAnywhere, Category = Setup)
    TSubclassOf<AAsteroid> AsteroidBlueprint;

    /** Normalised float value dictating the percentage progression on the spline at which a note should be hit at*/
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float NoteHitLocationOnSpline = 1.0f;
};

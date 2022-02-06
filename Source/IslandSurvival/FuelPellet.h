// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainGameState.h"
#include "FuelPellet.generated.h"

UCLASS()
class ISLANDSURVIVAL_API AFuelPellet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuelPellet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	bool IsWithEditor();
private:
	AMainGameState* MainGameState;
	bool bCollided; //ensures can only collide once with the player
	UFUNCTION()
	void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);
};

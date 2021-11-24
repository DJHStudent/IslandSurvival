// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZombieSpawner.generated.h"

UCLASS()
class ISLANDSURVIVAL_API AZombieSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AZombieSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadOnly)
	float ZombieScale; //the key for the biome spawner is in	
	UPROPERTY(BlueprintReadOnly)
	float ZombieWalkSpeed; //the key for the biome spawner is in	
	UPROPERTY(BlueprintReadOnly)
	float ZombieSwimSpeed; //the key for the biome spawner is in

};

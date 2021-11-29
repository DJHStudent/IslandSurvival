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
	float ZombieScale; //the size of the zombie
	UPROPERTY(BlueprintReadOnly)
	float ZombieWalkSpeed; //the zombies walking speed	
	UPROPERTY(BlueprintReadOnly)
	float ZombieSwimSpeed; //the swim speed of the zombie
};

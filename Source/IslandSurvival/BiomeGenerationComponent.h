// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCMapV2.h"
#include "BiomeGenerationComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ISLANDSURVIVAL_API UBiomeGenerationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBiomeGenerationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1", ClampMax = "1"))
	float WaterLine;

	class APCMapV2* TerrainGenerator;
	void AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition);
	TMap<int32, TArray<int32>> IslandPointsMap; //a list of every island and the index of the vertices which belong to each one
	int32 IslandKeys;

	void ColourOfIsland();
private:
	//UPROPERTY(EditAnywhere)
	void JoinIslands(int32 IslandPoint, int32 NewPoint);

	int32 FindCorrectIsland(int32 Point);
};

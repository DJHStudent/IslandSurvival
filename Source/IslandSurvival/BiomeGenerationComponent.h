// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PCMapV2.h"
#include "BiomeGenerationComponent.generated.h"


USTRUCT()
struct FBiomeStats //for the noise based biomes
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString BiomeName;

	UPROPERTY(EditAnywhere)
	FLinearColor BiomeColour;

	FBiomeStats()
	{
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
	}
};
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


	//all related to determining the locations of all points on an island
	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1", ClampMax = "1"))
	float WaterLine;

	class APCMapV2* TerrainGenerator;

	void AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition);
	TMap<int32, TArray<int32>> IslandPointsMap; //a list of every island and the index of the vertices which belong to each one
	TArray<int32> BiomeAtEachPoint; //for each vertex of the map the biome which resides their, identified by its key value
	int32 IslandKeys;

	void ColourOfIsland();

	UPROPERTY(EditAnywhere)
	TMap<int32, FBiomeStats> DifferentBiomesMap; //a map is used so can gain easy access to any biome by simply using its key
private:
	//UPROPERTY(EditAnywhere)
	void JoinIslands(int32 IslandPoint, int32 NewPoint); //for when generating islands some are unjoined, join them together
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoissonDiskSampling.h"
#include "PCMapV2.h"
#include "BiomeStats.h"
#include "BiomeGenerationComponent.generated.h"


USTRUCT() struct FIslandStats //the different parameters for an island
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> VertexIndices;

	float MinXPosition;
	float MaxXPosition;
	float MinYPosition;
	float MaxYPosition;

	void UpdateIslandBounds(FVector2D NewSize) //as points get added update the variables as they change
	{
		if (NewSize.X < MinXPosition)
			MinXPosition = NewSize.X;
		if (NewSize.Y < MinYPosition)
			MinYPosition = NewSize.Y;

		if (NewSize.X > MaxXPosition)
			MaxXPosition = NewSize.X;
		if (NewSize.Y > MaxYPosition)
			MaxYPosition = NewSize.Y;
	}

	FIslandStats()
	{
		VertexIndices = TArray<int32>();

		MinXPosition = TNumericLimits<float>::Max();
		MinYPosition = TNumericLimits<float>::Max();
		MaxXPosition = TNumericLimits<float>::Max();
		MaxYPosition = TNumericLimits<float>::Max();
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

	//stuff for determining where each island is
	float AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition);
	UPROPERTY()
	TMap<int32, FIslandStats> IslandPointsMap; //a list of every island and the vertices contained within it, being their array index value within the vertices array
	//an island is just a set of any number of vertices which are joined together above the waterline
	TArray<int32> BiomeAtEachPoint; //for each vertex of the map the biome which resides their, identified by its key value
	int32 IslandKeys;

	void VerticesBiomes(); //for each island determine the biome(s) residing their


	UPROPERTY(EditAnywhere)
	TMap<int32, FBiomeStats> DifferentBiomesMap; //A map of the stats of each biome and the int key used to identify it, a map is used so can gain easy access to any biome by simply using its key

	void SpawnMeshes();
private:

	PoissonDiskSampling DiskSampling;

	//UPROPERTY(EditAnywhere)
	void JoinIslands(int32 IslandPoint, int32 NewPoint); //for when generating islands some are unjoined, join them together
	
	void UpdateBiomeLists(FLinearColor BiomeColour, int32 Biome, int32 VertexIdentifier);
	bool bHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier); //based on height of point, determine the biome 
	void SingleBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize); //islands below a certain size will have only 1 biome
	void MultiBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize); //islands below a certain size will have only 1 biome


	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))//the max size an island can be to have a single island
	float SingleIslandMaxSize; 

	TMap<int32, TArray<int32>> VertexBiomeLocationsMap; //a map where their is a list of each biome and every vertex which makes it up 
};
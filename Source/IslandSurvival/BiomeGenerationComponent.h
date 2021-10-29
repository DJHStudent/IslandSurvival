// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoissonDiskSampling.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "BiomeStats.h"
#include "BiomeGenerationComponent.generated.h"


USTRUCT() struct FIslandStats //the different parameters for an island
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<int32> VertexIndices; //a list of the indexes of the vertex array which make up the island

		//these points used to determine the position of biomes on the island
	float MinXPosition; //the smallest X vertex postion of the island
	float MaxXPosition; //the largest X vertex postion of the island
	float MinYPosition; //the smallest Y vertex postion of the island
	float MaxYPosition; //the largest Y vertex postion of the island

	void UpdateIslandBounds(FVector2D NewSize) //as points get added update the islands min/max positions accordingly
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

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1", ClampMax = "1")) //the Z position of water on the map
	float WaterLine;

	class AProcedurallyGeneratedTerrain* TerrainGenerator;

	void AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition); //for each point determine the island it relates too
	UPROPERTY()
	TMap<int32, FIslandStats> IslandPointsMap; //a map containing a key for the specific island it is and its various statistics
	//an island is just a set of any number of vertices which are joined together above the waterline
	int32 IslandKeys; //the current max key have, ensuring no duplicates are created

	TArray<int32> BiomeAtEachPoint; //for each vertex of the map the biome which resides their, identified by its key value
	void VerticesBiomes(); //for each island determine the biome(s) residing within it

	UPROPERTY(EditAnywhere)
	TMap<int32, TSubclassOf<UBiomeStatsObject>> BiomeStatsMap;


	void SpawnMeshes(); //for each biome spawn in the appropriate meshes
	TMap<int32, TArray<int32>> VertexBiomeLocationsMap; //a map where the key is the biomes key it relates to and the index of every vertex held within 
	UPROPERTY()
	TArray<AActor*> MeshActors; //all meshes added into the world

	void BiomeLerping(int32 i, int32 j);
	TArray<bool> bBeenLerped;

	void SpawnTents();
private:

	PoissonDiskSampling DiskSampling;

	void JoinIslands(int32 IslandPoint, int32 NewPoint); //for when generating islands some are unjoined and disconected, so join them together
	
	void UpdateBiomeLists(int32 Biome, int32 VertexIdentifier); 
	bool bHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier); //determine if it is a height based biome or not
	void SingleBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize); //islands below a certain size will have only 1 biome
	void MultiBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize); //for all biomes above a certain size generate multiple biomes

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))//the max size an island can be to have a single biome
	float SingleIslandMaxSize; 

	FVector MeshLocation(FVector VertexPosition);

	UPROPERTY(EditAnywhere)//the mesh to spawn in
		UStaticMesh* Tent;
	UPROPERTY(EditAnywhere)//the mesh to spawn in
		UStaticMesh* Bouy;
};
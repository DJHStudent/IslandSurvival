// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoissonDiskSampling.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "BiomeStats.h"
#include "BiomeGenerationComponent.generated.h"

UENUM()
enum EVertexSpawnLocation
{
	Land = 1,
	Water = -1,
};

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

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1")) //the max number of vertices away from the border of a biome blending will occur
													//note: only if blending enabled for the biome
	int32 BlendAmount;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1", ClampMax = "1")) //the Z position of water on the map
																	 //note: multiplied by the base terrains perlin scale to get the water height
	float WaterLine;

	class AProcedurallyGeneratedTerrain* TerrainGenerator;

	void AddBiomePoints(const int32 XPosition, const int32 YPosition, const float ZPosition);

	void AddSinglePoint(const int32 XPosition, const int32 YPosition, const int32 SpawnLocation);

	TArray<int32> SingleLandBiomeKeys; //list of all biomes with the spawn condition set to land
	TArray<int32> MultiLandBiomeKeys; //list of all biomes with the spawn condition set to land
	TArray<int32> SingleLakeBiomeKeys; //list of all biomes which appear underwater
	TArray<int32> MultiLakeBiomeKeys; //list of all biomes which appear underwater
	TArray<int32> HeightBiomeKeys;

	//list of all the features(islands/lakes) and the vertices related to each one
	UPROPERTY() //for this to work, negative keys for lake and positive keys for island
	TMap<int32, FIslandStats> FeaturePointsMap;//a map containing a key for the specific island it is and its various statistics
	int32 FeatureKeys;//the current max key have, ensuring no duplicates are created



	void DeterminePointBiomes();


	TArray<int32> BiomeAtEachPoint; //for each vertex of the map the biome which resides their, identified by its key value
	//for each island determine the biome(s) residing within it
	void VerticesBiomes();

	UPROPERTY(EditAnywhere) //The biome blueprints to spawn in, each containing a unique integer key
	TMap<int32, TSubclassOf<UBiomeStatsObject>> BiomeStatsMap;

	void SpawnMeshes(); //for each biome spawn in the appropriate meshes
	TMap<int32, TArray<int32>> VertexBiomeLocationsMap; //a map where the key is the biomes key it relates to and the index of every vertex held within 
	UPROPERTY()
	TArray<AActor*> MeshActors; //all meshes added into the world

	void BiomeLerping(int32 i, int32 j); //for two biomes, if at border blend the height values together so get a smoother transition
	TArray<TPair<bool, float>> bBeenLerped; //has this vertex been succesfully blended with its neighbour yet + original Z value of point
	void BiomeBlending();

	void SpawnStructure(); //spawn in tents and bouys around the map
private:
	void JoinPoints(const int32 IslandPoint, const int32 NewPoint); //for when generating islands some are unjoined and disconected, so join them together
	
	void UpdateBiomeLists(int32 Biome, int32 VertexIdentifier); 
	bool HasHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier); //determine if it is a height based biome or not
	
	void EachPointsMap();
	void SingleBiomePoints(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize, TArray<int32>& SingleBiomeKeys, TArray<int32>& MultiBiomeKeys); //islands below a certain size will have only 1 biome
	//for all biomes above a certain size generate multiple biomes
	void MultiBiomePoints(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize, TArray<int32>& MultiBiomeKeys);

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))//the max rectangular area an island / lake can have to only have a single biome
												   //note: doesn't apply to height based biomes
	float SingleFeatureMaxSize; 

	FVector MeshLocation(FVector VertexPosition); //determine the offset to give the plant mesh
	//spawn in a zombie spawner
	void SpawnZombieSpawner(FVector Location, int32 Index); 

	UPROPERTY(EditAnywhere)//the tent mesh
		UStaticMesh* Tent;
	UPROPERTY(EditAnywhere)//the bouy mesh
		UStaticMesh* Bouy;
};
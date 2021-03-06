// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/UObjectGlobals.h"
#include "Engine/StaticMesh.h"
#include "TerrainHeight.h"
#include "BiomeStats.generated.h"

/**
 * 
 */
//a file containing some structs used for holding different information for multiple arrays in the BiomeGenerationComponent class
UENUM()
namespace EBiomeStats
{
	enum Type //this way so the enum can appear within the editor
	{
		//spawn biome on the land only
		LandBased UMETA(DisplayName = "Land Based"),
		//spawn biome in the water only
		WaterBased UMETA(DisplayName = "Water Based"),
		//spawn biome based on terrain height, overrides other spawning
		HeightBased UMETA(DisplayName = "Height Based"),
	};
}

USTRUCT() struct FBiomeMeshes //Any stat nessesary for the different plants in a biome
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))//% of the vertices of this biome taken up by this mesh
	float Density;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))//the max vertices away from a random point within the biome can
												   //a random amount of the mesh spawn in 
	int32 NeighbourRadius;
	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))//the min number of the mesh which spawn within this radius
	int32 MinNeighbours;	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0")) //the max number of the mesh which can spawn in within this radius
	int32 MaxNeighbours;

	UPROPERTY(EditAnywhere) //does the mesh have any collision properties
	bool bHasCollision;
	UPROPERTY(EditAnywhere)//the mesh to spawn in
	UStaticMesh* Mesh;


	FBiomeMeshes()
	{
		Density = 0;
		bHasCollision = true;
	}
};

UCLASS(Blueprintable) //allow class to be instanced and assigned in the details panel
class ISLANDSURVIVAL_API UBiomeStatsObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere) //the name of the biome to be displayed
	FString BiomeName;

	UPROPERTY(EditAnywhere) //the colour of each vertex of the biome
	FLinearColor BiomeColour;

	UPROPERTY(EditAnywhere) //a list of the different meshes to spawn in in this biome
	TArray<FBiomeMeshes> BiomeMeshes;

	UPROPERTY(EditAnywhere) //the keys of the different biomes this one can only spawn next to
						    //keys must have same spawn location or be height based
						    //note: leaving it empty will make any biome be a valid neighbour
	TArray<int32> NeighbourBiomeKeys;

	UPROPERTY(EditAnywhere, Category = "Biome Spawning") //the inital spawning location parameter(s) for the biome
	TEnumAsByte<EBiomeStats::Type> BiomeSpawningEnum;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning") //the min height the biome spawns in
	float MinSpawnHeight;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning") //the max height the biome spawns in
	float MaxSpawnHeight;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning") //does this biome only appear where land/ water areas are smaller than Single Feature Max Size
	bool bOnlySingle;
	UPROPERTY(EditAnywhere, Category = "Biome Spawning", meta = (ClampMin = "0")) //the min rectangular area it needs to spawn in
	float MinSpawnArea;
	UPROPERTY(EditAnywhere, Category = "Biome Spawning", meta = (ClampMin = "0")) //the max rectangular area it can spawn in
	float MaxSpawnArea;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning") //at the biome's edges, should blending with neighbour terrain occur
	bool bDoBlending;

	float SpawnChance;

	UPROPERTY(EditAnywhere, Category = "Terrain Height") //should this biome override the default terrain with its own custom values
	bool bCustomTerrain;
	
	UPROPERTY(Instanced, EditAnywhere, Category = "Terrain Height") //the settings for adjusting the custom terrain
	UTerrainHeight* TerrainHeight;

	UPROPERTY(EditAnywhere, Category = "Zombie Stats") //Size of the Zombies which Spawn here
	float Scale;
	UPROPERTY(EditAnywhere, Category = "Zombie Stats")//How fast Zombies spawning here can walk/run
	float WalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Zombie Stats")//How fast zombie spawning here can swim
	float SwimSpeed;

	UBiomeStatsObject()
	{
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
		bCustomTerrain = false;

		Scale = 1;
		WalkSpeed = 600;
		SwimSpeed = 1000.0;

		MaxSpawnArea = 6000.0;

		SpawnChance = 100;
		bDoBlending = true;
	}
};
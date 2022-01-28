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
		LandBased UMETA(DisplayName = "Land Based"),
		WaterBased UMETA(DisplayName = "Water Based"),
		HeightBased UMETA(DisplayName = "Height Based"),
	};
}

USTRUCT() struct FBiomeMeshes //Any stat nessesary for the different plants in a biome
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))//% out of 100 of the biomes area taken up by this model
	float Density;

	//how far away can a neighbour mesh actually spawn(in grid sections)
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int32 NeighbourRadius;
	//as when spawning in, how many neighbours can actually spawn around it
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0"))
	int32 MinNeighbours;	
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0")) //FMath::Pow(NeighbourRadius * 2, 2)) for max neighbours
	int32 MaxNeighbours;


	UPROPERTY(EditAnywhere)//the mesh to spawn in
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere) //can the player collide with the specified mesh
	bool bHasCollision;

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
	UPROPERTY(EditAnywhere) //the name of the biome to be displayed whenever in it
	FString BiomeName;

	UPROPERTY(EditAnywhere) //the colour of each vertex when inside the biome
	FLinearColor BiomeColour;

	UPROPERTY(EditAnywhere) //a list of the different meshes and their related components to spawn in in this biome
	TArray<FBiomeMeshes> BiomeMeshes;

	UPROPERTY(EditAnywhere) //a list containing the key of each biome which can spawn in next to this one
	TArray<int32> NeighbourBiomeKeys;

	UPROPERTY(EditAnywhere, Category = "Biome Spawning")
	TEnumAsByte<EBiomeStats::Type> BiomeSpawningEnum; //allows the enum to appear within the editor
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning")
	float MinSpawnHeight;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning")
	float MaxSpawnHeight;
	
	UPROPERTY(EditAnywhere, Category = "Biome Spawning")
	bool bOnlySingle; //only appear on the small islands
	UPROPERTY(EditAnywhere, Category = "Biome Spawning", meta = (ClampMin = "0"))
	float MinSpawnArea;
	UPROPERTY(EditAnywhere, Category = "Biome Spawning", meta = (ClampMin = "0"))
	float MaxSpawnArea;
	
	float SpawnChance; //biome with heighest value spawns first

	UPROPERTY(EditAnywhere, Category = "Terrain Height") //for each biome should it have a custom terrain, or use the default terrain made when first generated
	bool bCustomTerrain;
	
	UPROPERTY(Instanced, EditAnywhere, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

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
	}
};
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

USTRUCT() struct FBiomeMeshes //Any stat nessesary for the different plants in a biome
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))//% out of 100 of the biomes area taken up by this model
	float Density;
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

UCLASS(Blueprintable, EditInlineNew) //allow class to be instanced and assigned in the details panel
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

	UPROPERTY(EditAnywhere, Category = "Terrain Height") //for each biome should it have a custom terrain, or use the default terrain made when first generated
	bool bCustomTerrain;
	
	UPROPERTY(Instanced, EditAnywhere, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

	UBiomeStatsObject()
	{
		//TerrainHeight = NewObject<UTerrainHeight>(GetOwner(), TEXT("Terrain Height"));
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
		bCustomTerrain = false;
	}
};
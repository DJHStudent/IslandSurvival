// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StaticMesh.h"
#include "BiomeStats.generated.h"

/**
 * 
 */

USTRUCT() struct FBiomeMeshes //Any stat nessesary for the different plants in a biome
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))//% out of 100 of the biomes area taken up by this model
		float Density;
	UPROPERTY(EditAnywhere)//the mesh to spawn in
		UStaticMesh* Mesh;

	FBiomeMeshes()
	{
		Density = 0;
	}
};

USTRUCT()
struct FBiomeStats//for the noise based biomes
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FString BiomeName;

	UPROPERTY(EditAnywhere)
	FLinearColor BiomeColour;

	UPROPERTY(EditAnywhere)
	float BiomeRarity; //how likly is this biome to spawn in
	float CurrentRarity; //the more this biome spawns in the rarer it gets

	UPROPERTY(EditAnywhere)
	TArray<FBiomeMeshes> BiomeMeshes;

	UPROPERTY(EditAnywhere)
	TArray<int32> NeighbourBiomeKeys; //a list of biomes which can spawn around this one

	FBiomeStats()
	{
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
		BiomeRarity = 0;
		CurrentRarity = BiomeRarity;
	}
};
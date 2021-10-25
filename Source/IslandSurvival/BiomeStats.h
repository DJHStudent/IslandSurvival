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

USTRUCT() struct FBiomeHeight
{
	GENERATED_BODY()
	//float GenerateHeight(int32 XPosition, int32 YPosition); //returns the height of each vertex in turn
	UPROPERTY(EditAnywhere)
		float TerraceSize;
	TArray<float> OcataveOffsets;

	UPROPERTY(EditAnywhere)
	UTerrainHeight* TerrainHeight;
//private:
	UPROPERTY(EditAnywhere)//the range of values perlin noise will return
		float PerlinScale;
	UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
		float PerlinRoughness;

	UPROPERTY(EditAnywhere, Category = "FBM")//number of perlin noise maps to layer
		int32 Octaves;
	UPROPERTY(EditAnywhere, Category = "FBM")//the gap in frequency from one layer to the next
		float Lacunarity;
	UPROPERTY(EditAnywhere, Category = "FBM")//how the amplitude changes over time
		float Grain;

	//float FractalBrownianMotion(int32 XPosition, int32 YPosition);

	//float DomainWarping(float XPos, float YPos); //offset the vertices of each point by specific values
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//the amount of offset to actually apply to each vertex
		float DomainAmount;	
	UPROPERTY(EditAnyWhere)//the amount of Height offset to actually apply to each vertex
		float HeightOffsetValue;

	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoTerrace;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoPower;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoRigid;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoBillowy;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoMeshes;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoIsland;

	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsPower;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsRigid;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsBillowy;


	float FractalBrownianMotion(int32 XPosition, int32 YPosition)
	{
		float HeightSum = 0; //the sum of the height at each octave
		float Frequency = 1; //offset value for steepness of each successive octave
		float Amplitude = 1; //the offset value for the height at each octave

		for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
		{
			//for each octave determine the noise value to use, using frequency
			float NoiseValue = (FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffsets[i], YPosition + OcataveOffsets[i]) * Frequency * PerlinRoughness)); 
			//nose in form 0-1 so above water

			HeightSum += NoiseValue * Amplitude; //add this noise value to the total with amplitude

			Frequency *= Lacunarity; //the influence of the frequency over each sucessive octave, increasing
			Amplitude *= Grain; //influence of amplitude on each sucessive octave, decreasing
		}
		return HeightSum; //return the final height sum
	}


	float DomainWarping(float XPosition, float YPosition) //for each vertex offset its height by a specific amount of values, through combining multiple FBM noise
	{
		//calculate the firest points X and Y position
		FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
		//determine the next points X and Y position based on q's point values
		FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));

		//Determine the final new height to give the point
		float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);

		return NewHeight;
	}


	float GenerateHeight(int32 XPosition, int32 YPosition) //all the functions for determining the height of a specific point
	{
		float FBMValue = (DomainWarping(XPosition, YPosition) + 1) / 2; //determine the inital value of the point using domain warping

		float HeightValue = FBMValue;
		if (bDoPower || bIsPower)
			HeightValue *= FMath::Pow(FBMValue, 2.0f); //this will give us terrain which consists mostly of flater land broken up occasionally by hills and valleys
		if (bDoBillowy)
			HeightValue *= FMath::Abs(FBMValue); //this will add more rolling hills
		if (bIsBillowy)
			HeightValue = FMath::Abs(HeightValue);
		if (bDoRigid)
			HeightValue *= 1 - FMath::Abs(FBMValue); //this will add sharp peaks or ridges as a possibility to occur
		if (bIsRigid)
			HeightValue = 1 - FMath::Abs(HeightValue);

		if (bDoTerrace)
			HeightValue = FMath::RoundFromZero(HeightValue * TerraceSize) / TerraceSize;//terrace the terrain by rouding each points height to its nearest multiple of TerraceSize

		HeightValue *= PerlinScale; //give the Z position its final in game height

		return HeightValue + HeightOffsetValue;
	}
	FBiomeHeight() //empty constructor for the class
	{
		//TerrainHeight = NewObject<UTerrainHeight>(this, TEXT("Terrain Height"), EObjectFlags::RF_Public);
		TerraceSize = 15;
		PerlinScale = 2000;
		PerlinRoughness = 0.01f;
		Octaves = 6;
		Lacunarity = 2.0f;
		Grain = 0.5f;
		DomainAmount = 10.0f;
		bDoTerrace = true;
		bIsBillowy = true;
		HeightOffsetValue = 0;
	}
};

USTRUCT()
struct FBiomeStats//the stats for each biome which spawns in
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere) //the name of the biome to be displayed whenever in it
	FString BiomeName;

	UPROPERTY(EditAnywhere) //the colour of each vertex when inside the biome
	FLinearColor BiomeColour;

	UPROPERTY(EditAnywhere) //a list of the different meshes and their related components to spawn in in this biome
	TArray<FBiomeMeshes> BiomeMeshes;

	UPROPERTY(EditAnywhere) //a list containing the key of each biome which can spawn in next to this one
	TArray<int32> NeighbourBiomeKeys;

	UPROPERTY(EditAnywhere)
	struct FBiomeHeight BiomeHeight;

	UPROPERTY(Instanced, EditAnywhere, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

	FBiomeStats()
	{
		//TerrainHeight = NewObject<UTerrainHeight>(GetOwner(), TEXT("Terrain Height"));
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
	}
};

UCLASS(Blueprintable, EditInlineNew) //allow class to be instanced and assigned in the details panel
class ISLANDSURVIVAL_API UBiomeStatsObject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere) //the name of the biome to be displayed whenever in it
		FString BiomeName;

	UPROPERTY(EditAnywhere) //the colour of each vertex when inside the biome
		FLinearColor BiomeColour;

	UPROPERTY(EditAnywhere) //a list of the different meshes and their related components to spawn in in this biome
		TArray<FBiomeMeshes> BiomeMeshes;

	UPROPERTY(EditAnywhere) //a list containing the key of each biome which can spawn in next to this one
		TArray<int32> NeighbourBiomeKeys;

	UPROPERTY(EditAnywhere)
	struct FBiomeHeight BiomeHeight;

	UPROPERTY(Instanced, EditAnywhere, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

	UBiomeStatsObject()
	{
		//TerrainHeight = NewObject<UTerrainHeight>(GetOwner(), TEXT("Terrain Height"));
		BiomeName = FString(TEXT(""));
		BiomeColour = FLinearColor(0, 0, 0);
	}
};
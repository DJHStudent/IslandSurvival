// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TerrainHeight.generated.h"

/**
 * 
 */

UENUM()
namespace ENoiseDepth //the amount of the noise to actually generate
{
	enum Type
	{	//all vertices of the biome can appear above and below the waterline
		FullNoise UMETA(DisplayName = "Full Height"),
		//all vertices of the biome will appear above the waterline
		LandNoise UMETA(DisplayName = "Above Water Only"),
		//all vertices of the biome will appear below the waterline
		WaterNoise UMETA(DisplayName = "Below Water Only"),
	};
}

UENUM()
namespace ETerrainAdditions //the amount of the noise to actually generate
{
	enum Type
	{	
		//don't apply this feature to the terrain
		DontUse UMETA(DisplayName = "Don't Use"),
		//convert the current Z position to become this feature
		FullUse UMETA(DisplayName = "Convert Current Terrain to Use this"),
		//add this with the current Z position, creating a possibility for it to appear
		Add UMETA(DisplayName = "Add to Existing"),
		//subtract this with the current Z position, creating a possibility for it to appear
		Subtract UMETA(DisplayName = "Subtract from Existing"),
		//multiply this with the current Z position, creating a possibility for it to appear
		Multiply UMETA(DisplayName = "Multiply with Existing"),
		//divide this with the current Z position, creating a possibility for it to appear
		Divide UMETA(DisplayName = "Divide with Existing"),
	};
}


UCLASS(Blueprintable, EditInlineNew) //allow class to be instanced and assigned in the details panel
class ISLANDSURVIVAL_API UTerrainHeight : public UObject
{ //class used to store any values specific for the noise function doing on the biome / terrain as a whole
	GENERATED_BODY()

public:
	// Called every frame
	float GenerateHeight(const int32 XPosition, const int32 YPosition, const float WaterZPos, const bool bSmooth); //returns the height of each vertex in turn
	
	void DeclareOffsetValues(FRandomStream Stream); //probably should actually delcare them here
	
	int32 Width; //width and height of of the terrain
	int32 Height; 
private:
	//the random values to offset the perlin noise by in each octave, in order to introduce randomness
	TArray<float> OctaveOffsets;

	UPROPERTY(EditAnywhere) //the Z position constraint for the vertices of the biome
	TEnumAsByte<ENoiseDepth::Type> NoiseDepthEnum; //allows the enum to appear within the editor
public:

	UPROPERTY(EditAnywhere)//the range, both positive and negative, the vertices Z position will be within
	float PerlinScale;
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "1"))//how smooth or spiky the terrain will be
	float PerlinRoughness;
	
	UPROPERTY(EditAnywhere, Category = "FBM")//number of layers of perlin noise to use
	int32 Octaves;
	UPROPERTY(EditAnywhere, Category = "FBM")//the linear change in Perlin Roughness over each successive octave
											//i.e: the change in frequency of each octave of the noise
	float Lacunarity;
	UPROPERTY(EditAnywhere, Category = "FBM", meta = (ClampMin = "0", ClampMax = "1"))//the linear change in influence of each successive octave
																					 //i.e: the change in amplitude of each octave of the noise
	float Grain;

	float FractalBrownianMotion(int32 XPosition, int32 YPosition);
	
	//offset the vertices of each point by specific values
	float DomainWarping(const int32 XPosition, const int32 YPosition);
	UPROPERTY(EditAnyWhere, Category = "Domain Warping", meta = (ClampMin = "0"))//the amount of random distortion to apply to each vertex
	float DomainAmount;
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//should this biome use domain warping
	bool bDoWarping;

	UPROPERTY(EditAnywhere, Category = "Terraces", meta = (ClampMin = "0"))//should this biome be terraced 
	bool bDoTerrace;	
	UPROPERTY(EditAnywhere, Category = "Terraces", meta = (ClampMin = "0")) //the inverse distance appart of each terrace
																			//i.e: larger values mean smaller appart
	float TerraceSize;

	float SquareGradient(const int32 XPosition, const int32 YPosition);
	UPROPERTY(EditAnywhere, Category = "FallOff")//should the map, falloff around the edges to create islands surounded by water
	bool bDoFalloff;
	UPROPERTY(EditAnywhere, Category = "FallOff")//distance from the centre of the map where the falloff begins
	float DistStarts;
	UPROPERTY(EditAnywhere, Category = "FallOff")//steepness of the transition from land to water
	float Steepness;
	UPROPERTY(EditAnywhere, Category = "FallOff", meta = (ClampMin = "0"))//amound of the map, from centre point which is above water
	float AboveWater;

	UPROPERTY(EditAnywhere, Category = "Noise Types") //flattens the terrain so only large features like mountains and valleys appear every so often
	TEnumAsByte<ETerrainAdditions::Type> PowerNoiseEnum;

	UPROPERTY(EditAnywhere, Category = "Noise Types")//Allow rigdes to appear
	TEnumAsByte<ETerrainAdditions::Type> RigidNoiseEnum;

	UPROPERTY(EditAnywhere, Category = "Noise Types")//Allow rolling hills to appear
	TEnumAsByte<ETerrainAdditions::Type> BillowyNoiseEnum; //allows the enum to appear within the editor

	float TerrainAdditionMode(ETerrainAdditions::Type AddMode, const float CurrentValue, const float AdditionalValue);
};

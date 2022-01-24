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
	{	//noise between -1 and 1
		FullNoise UMETA(DisplayName = "Full Height"),
		//noise between 0 and 1
		LandNoise UMETA(DisplayName = "Above Water Only"),
		//noise between -1 and 0
		WaterNoise UMETA(DisplayName = "Below Water Only"),
	};
}

UENUM()
namespace ETerrainAdditions //the amount of the noise to actually generate
{
	enum Type
	{	
		DontUse UMETA(DisplayName = "Don't Use"),
		FullUse UMETA(DisplayName = "Convert Current Terrain to Use this"),
		Add UMETA(DisplayName = "Add to Existing"),
		Subtract UMETA(DisplayName = "Subtract from Existing"),
		Multiply UMETA(DisplayName = "Multiply with Existing"),
		Divide UMETA(DisplayName = "Divide with Existing"),
	};
}


UCLASS(Blueprintable, EditInlineNew) //allow class to be instanced and assigned in the details panel
class ISLANDSURVIVAL_API UTerrainHeight : public UObject
{ //class used to store any values specific for the noise function doing on the biome / terrain as a whole
	GENERATED_BODY()

public:
	// Called every frame
	float GenerateHeight(int32 XPosition, int32 YPosition, float WaterZPos, bool bSmooth); //returns the height of each vertex in turn
	
	void DeclareOffsetValues(FRandomStream Stream); //probably should actually delcare them here
	
	int32 Width; //width and height of of the terrain
	int32 Height; 
private:
	//the random values to offset the perlin noise by in each octave, in order to introduce randomness
	TArray<float> OctaveOffsets;

	UPROPERTY(EditAnywhere) //is the terrain only above or below the centre area or not
	TEnumAsByte<ENoiseDepth::Type> NoiseDepthEnum; //allows the enum to appear within the editor
public:
	UPROPERTY(EditAnywhere, Category = "FBM")//number of perlin noise maps to layer
		int32 Octaves;
	UPROPERTY(EditAnywhere)//the range of values perlin noise will return
		float PerlinScale;
	UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
		float PerlinRoughness;

	UPROPERTY(EditAnywhere, Category = "FBM")//the gap in frequency from one layer to the next
		float Lacunarity;
	UPROPERTY(EditAnywhere, Category = "FBM")//how the amplitude changes over time
		float Grain;

	float FractalBrownianMotion(int32 XPosition, int32 YPosition);

	float DomainWarping(float XPos, float YPos); //offset the vertices of each point by specific values
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//the amount of offset to actually apply to each vertex
		float DomainAmount;
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//the amount of offset to actually apply to each vertex
		bool bDoWarping;

	UPROPERTY(EditAnywhere, Category = "Terraces") //the distance appart of each terrace, larger value means smaller appart
		bool bDoTerrace;	
	UPROPERTY(EditAnywhere, Category = "Terraces")
		float TerraceSize;

	float SquareGradient(float XPosition, float YPosition);
	UPROPERTY(EditAnywhere, Category = "FallOff") //amound of the map, from centre point is above water
		bool bDoFalloff;
	UPROPERTY(EditAnywhere, Category = "FallOff")//steepness of the transition from island to deep abyss underwater
		float Steepness;
	UPROPERTY(EditAnywhere, Category = "FallOff") //controls how much of the map's border is actually underwater
		float Size;
	UPROPERTY(EditAnywhere, Category = "FallOff") //amound of the map, from centre point is above water
		float AboveWater;



	//UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
	//	bool bDoPower;
	//UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
	//	bool bDoRigid;
	//UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
	//	bool bDoBillowy;
	////UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller apparts
	////	bool bDoIsland;

	//UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
	//	bool bIsPower;
	UPROPERTY(EditAnywhere, Category = "Noise Types") //is the terrain only above or below the centre area or not
		TEnumAsByte<ETerrainAdditions::Type> PowerNoiseEnum; //allows the enum to appear within the editor
	//UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
	//	bool bIsRigid;
	UPROPERTY(EditAnywhere, Category = "Noise Types") //is the terrain only above or below the centre area or not
		TEnumAsByte<ETerrainAdditions::Type> RigidNoiseEnum; //allows the enum to appear within the editor
	//UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
	//	bool bIsBillowy;
	UPROPERTY(EditAnywhere, Category = "Noise Types") //is the terrain only above or below the centre area or not
		TEnumAsByte<ETerrainAdditions::Type> BillowyNoiseEnum; //allows the enum to appear within the editor

	float TerrainAdditionMode(ETerrainAdditions::Type AddMode, float CurrentValue, float AdditionalValue);
};

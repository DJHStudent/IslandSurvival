// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TerrainHeight.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew) //allow class to be instanced and assigned in the details panel
class ISLANDSURVIVAL_API UTerrainHeight : public UObject
{
	GENERATED_BODY()

public:
	// Called every frame
	float GenerateHeight(int32 XPosition, int32 YPosition); //returns the height of each vertex in turn
	
	void DeclareOffsetValues(FRandomStream Stream); //probably should actually delcare them here
	
private:
	TArray<float> OctaveOffsets;


	UPROPERTY(EditAnywhere) //should the noise be between -1 and 1 or not(0 and 1)
		bool bFullNoise;

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



	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoPower;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoRigid;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoBillowy;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller apparts
		bool bDoIsland;

	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsPower;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsRigid;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsBillowy;
	
};

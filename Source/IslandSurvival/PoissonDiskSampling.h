// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "FBiomeStats.h"
#include "BiomeStats.h"
/**
 * 
 */
class ISLANDSURVIVAL_API PoissonDiskSampling //rename to DetermineBiomeLocations
{
public:
	PoissonDiskSampling();
	~PoissonDiskSampling();
	//radius is the min distance appart a biome can be from another, test values and try initially with the min island max size variable
	//k is number of points to test before algorithm rejects it typically k = 30 is fine
//	const TMap<int32, FBiomeStats>& DifferentBiomesMap;
	TArray<TPair<int32, FVector2D>> CreatePoints(const float& Radius, const int32& k, const float& IslandWidth, const float& IslandHeight, const float& XOriginOffset, const float& YOriginOffset, const TMap<int32, FBiomeStats>& DifferentBiomesMap); //create all the biomes points
	//bool bIsValid(FVector2D CandidatePoint, int32 GridPosition, int32 GridXSize, int32 CellSize);

private:
	int32 DetermineBiome(int32 NeighbourBiome, const TMap<int32, FBiomeStats>& DifferentBiomesMap); //for each point determine the biome which resides within it
};

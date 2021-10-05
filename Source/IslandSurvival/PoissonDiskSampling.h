// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	TArray<TPair<int32, FVector2D>> CreatePoints(const float& Radius, const int32& k, const float& IslandWidth, const float& IslandHeight, const float& XOriginOffset, const float& YOriginOffset); //create all the biomes points
	//bool bIsValid(FVector2D CandidatePoint, int32 GridPosition, int32 GridXSize, int32 CellSize);
};

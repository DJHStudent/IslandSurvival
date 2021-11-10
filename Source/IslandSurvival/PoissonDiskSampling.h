// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BiomeStats.h"
/**
 * 
 */
static class ISLANDSURVIVAL_API PoissonDiskSampling //rename to DetermineBiomeLocations
{
public:
	PoissonDiskSampling();
	~PoissonDiskSampling();
	/*
		how this actually works
		based on a grid where for each square only one point can resid inside it
		picks a random sqaure as starting point and 2 * radius away spawns a point
		determine the grid tile new point relates to and check 1 tile in each direction from it, with any other point existing within it being < radius from this new point causing
		a need to repeate upto k other points
		once a point found determine its biome and end

		Radius: the diagonal length of a grid square i.e the min distance appart two biomes can be
		K: Number of neighbour to check before it needs to discard it as no longer valid 
		Island Width/ Height: is total size of the grid which will be placed over the island
		X/Y origin offset: as grid is based around 0,0 need to offset the final islands point in order to get its actual position on the map
	*/

	static TArray<TPair<int32, FVector2D>> CreatePoints(const float& Radius, const int32& k, const float& IslandWidth, const float& IslandHeight, const float& XOriginOffset, const float& YOriginOffset, const TMap<int32, TSubclassOf<UBiomeStatsObject>>& DifferentBiomesMap, FRandomStream& Stream); //create all the biomes points

private:
	static int32 DetermineBiome(int32 NeighbourBiome, const TMap<int32, TSubclassOf<UBiomeStatsObject>>& DifferentBiomesMap, FRandomStream& Stream); //for each point determine the biome which resides within it
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "PoissonDiskSampling.h"
#include "Containers/Queue.h"

PoissonDiskSampling::PoissonDiskSampling()
{
}

PoissonDiskSampling::~PoissonDiskSampling()
{
}

TArray<TPair<int32, FVector2D>> PoissonDiskSampling::CreatePoints(const float& Radius, const int32& k, const float& IslandWidth, 
	const float& IslandHeight, const float& XOriginOffset, const float& YOriginOffset, const TMap<int32, 
	TSubclassOf<UBiomeStatsObject>>& DifferentBiomesMap, FRandomStream& Stream, TArray<int32>& BiomeKeys)
{	
	float SquaredRadius = FMath::Square(Radius);

	TArray<TPair<int32, FVector2D>> BiomePoints; //a list of points which contain the key for a biome
	/*
		Setup the grid of a specific size with default values
	*/
	TArray<FVector2D> GridPoints; //the vectors, each of which lie inside a single grid square
	TArray<int32> GridBiomes; //on the grid at the specified location, the biome, -1 means no biome their
	float CellSize = Radius / FMath::Sqrt(2); //on the actual grid, the size of a cell

	//determine the width and height of the grid of points, using the actual space avaliable(IslandSize) and the size of each cell(CellSize)
	int32 GridWidth = FMath::Clamp(FMath::CeilToInt(IslandWidth / CellSize), 1, TNumericLimits<int32>::Max()); //round down to nearest whole number so can fit into an array
	int32 GridHeight = FMath::Clamp(FMath::CeilToInt(IslandHeight / CellSize), 1, TNumericLimits<int32>::Max()); //clamped at 1 so no matter what size have, at least 1 grid sqaure exists

	//initilize the grid with default values and a specific predetermined size
	GridPoints.Init(FVector2D::ZeroVector, GridWidth * GridHeight);
	GridBiomes.Init(-1, GridWidth * GridHeight);

	/*
		Add an inital first value to the grid
	*/
	//set first biome point to be just the centre of the island
	float InitalXValue = IslandHeight / 2; 
	float InitalYValue = IslandWidth / 2;

	//determine the index of the point within the grid i.e the grid cell the point belongs too
	int32 XPosition = FMath::Clamp(FMath::FloorToInt(InitalXValue / CellSize), 0, GridWidth - 1);
	int32 YPosition = FMath::Clamp(FMath::FloorToInt(InitalYValue / CellSize), 0, GridHeight - 1);
	int32 InitialGridLocation = YPosition * GridWidth + XPosition;
	//add the inital point into the grid
	GridPoints[InitialGridLocation] = FVector2D(InitalXValue, InitalYValue);

	//determine the biome for the choosen point
	int32 NewBiome = DetermineBiome(GridBiomes[InitialGridLocation], DifferentBiomesMap, Stream, BiomeKeys);
	GridBiomes[InitialGridLocation] = NewBiome;

	//add the point as a valid biome location. Need the offset value as grid values centred around 0,0 so will shift the point to be around the islands location
	BiomePoints.Add(TPair<int32, FVector2D>(NewBiome, FVector2D(InitalXValue + XOriginOffset, InitalYValue + YOriginOffset)));


	//a list of all points which can still have neighbouring biomes placed around it
	TArray<int32> ActiveList; //holds the index of the biome's grid location within the GridPoints Array
	ActiveList.Add(InitialGridLocation); //add the initial grid location too it

	/*
		Add / check new points which get added until no new ones can exist
	*/
	while (ActiveList.Num() > 0 && GridWidth * GridHeight > 1) //while a point can still have neighbours continue to add/ check
	{
		//random active index of the grid
		int32 ActiveGridIndex = Stream.RandRange(0, ActiveList.Num() - 1);
		int32 ActiveGridIndexValue = ActiveList[ActiveGridIndex]; //get the value stored at this grid location
		FVector2D ActiveIndexLocation = GridPoints[ActiveGridIndexValue]; //get the vertex of the point within this grid location

		//test upto k points until a valid one around 2*Radius of the ActivePoint is found or all k return invalid
		bool bValidCandidate = false;
		for (int32 a = 0; a < k; a++)
		{
			//determine location of new point to check as anywhere in a circle around active location
			float Angle = Stream.FRandRange(0.0f, 1.0f) * PI * 2;
			FVector2D OffsetDirection = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle));//get a random direction to offset the current active point by
			float OffsetDistance = 2 * Radius; //the distance to offset the new point from the active point by
			
			//determine the location and cell the offset point belongs too
			FVector2D OffsetPosition = ActiveIndexLocation + OffsetDirection * OffsetDistance; //the new point will be an offset of the active point, in a random direction based on above parameters
			int32 OffsetGridXPosition = FMath::Clamp(FMath::FloorToInt(OffsetPosition.X / CellSize), 0, GridWidth - 1); //the position of this item within the grid, clamped between the min and max size of the grid
			int32 OffsetGridYPosition = FMath::Clamp(FMath::FloorToInt(OffsetPosition.Y / CellSize), 0, GridHeight - 1);

			//due to the setup only need to check the neighbours grid cell one cell away from the current point
			bool bOffsetValid = true;
			for (int32 i = -1; i <= 1; i++) //loop through all neighbouring grid points
			{
				for (int32 j = -1; j <= 1; j++)
				{
					//as long as the new point falls within the bounds of the island it can be used
					if (OffsetGridXPosition + j >= 0 && OffsetGridXPosition + j < GridWidth && OffsetGridYPosition + i >= 0 && OffsetGridYPosition + i < GridHeight)
					{
						int32 NeighbourGridIndex = (OffsetGridYPosition + i) * GridWidth + (OffsetGridXPosition + j);
						float Distance = FVector2D::DistSquared(OffsetPosition, GridPoints[NeighbourGridIndex]); //determine the distance between the active point and its neighbour's location
						if (GridBiomes[NeighbourGridIndex] != -1 && Distance < SquaredRadius) //if the offset point testing is too close to another already existing point
							bOffsetValid = false;
					}
					else //as outside array bounds it is also invalid so do not use this point for a new biome
						bOffsetValid = false;
				}
			}

			//if the offseted point is valid then add it as a new point
			if (bOffsetValid)
			{
				int32 OffsetGridIndex = (OffsetGridYPosition) * GridWidth + (OffsetGridXPosition); //get grid location to use
				GridPoints[OffsetGridIndex] = OffsetPosition; //assign the postion to it

				//determine the biome of the new point
				NewBiome = DetermineBiome(GridBiomes[ActiveGridIndexValue], DifferentBiomesMap, Stream, BiomeKeys);
				GridBiomes[OffsetGridIndex] = NewBiome;

				//add the point as a valid biome location
				BiomePoints.Add(TPair<int32, FVector2D>(NewBiome, FVector2D(OffsetPosition.X + XOriginOffset, OffsetPosition.Y + YOriginOffset)));
				ActiveList.Add(OffsetGridIndex); //add the index of the point to the active list

				bValidCandidate = true;
				break; //as valid point added, no need to do more searching with this point
			}

		}
		if (!bValidCandidate) //as no valid point was found, remove it from the active list so no new points can spawn around it
			ActiveList.Remove(ActiveGridIndexValue);
	}

	return BiomePoints; //return the list of points with appropriate biomes found
}

int32 PoissonDiskSampling::DetermineBiome(int32 NeighbourBiome, const TMap<int32, TSubclassOf<UBiomeStatsObject>>& DifferentBiomesMap, 
	FRandomStream& Stream, TArray<int32>& BiomeKeys)
{
	if (NeighbourBiome == -1 || DifferentBiomesMap[NeighbourBiome].GetDefaultObject()->NeighbourBiomeKeys.Num() == 0) //as no biome yet exists for the island just pick a random one
	{
		int32 RandomBiome = Stream.RandRange(0, BiomeKeys.Num() - 1);
		return BiomeKeys[RandomBiome];//the keys for the biomes not bound by specific conditions
	}
	else
	{
		//pick random biome from list of neighbouring biomes
		int32 RandBiome = Stream.RandRange(0, DifferentBiomesMap[NeighbourBiome].GetDefaultObject()->NeighbourBiomeKeys.Num() - 1); //also tie in the rarity system somehow
		return DifferentBiomesMap[NeighbourBiome].GetDefaultObject()->NeighbourBiomeKeys[RandBiome]; //return the value stored at the randomly choosen position within the array
	}
}
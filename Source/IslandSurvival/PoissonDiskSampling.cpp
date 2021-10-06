// Fill out your copyright notice in the Description page of Project Settings.


#include "PoissonDiskSampling.h"
#include "Containers/Queue.h"

PoissonDiskSampling::PoissonDiskSampling()
{
}

PoissonDiskSampling::~PoissonDiskSampling()
{
}

TArray<TPair<int32, FVector2D>> PoissonDiskSampling::CreatePoints(const float& Radius, const int32& k, const float& IslandWidth, const float& IslandHeight, const float& XOriginOffset, const float& YOriginOffset, const TMap<int32, FBiomeStats>& DifferentBiomesMap, FRandomStream& Stream)
{
	TArray<TPair<int32, FVector2D>> BiomePoints; //a list of points and the biomes which comprise them

	/*
		Setup the grid of a specific size with default values
	*/
	TArray<FVector2D> GridPoints; //the vectors, each of which lie inside a single grid square
	TArray<int32> GridBiomes; //on the grid at the specified location, the biome, -1 means no biome their
	float CellSize = Radius / FMath::Sqrt(2); //on the actual grid, the size of a cell

	//determine the width and height of the grid of points, using the actual space avaliable(IslandSize) and the size of each cell(CellSize)
	int32 GridWidth = FMath::Clamp(FMath::CeilToInt(IslandWidth / CellSize), 1, TNumericLimits<int32>::Max()); //round down to nearest whole number so can fit into an array
	int32 GridHeight = FMath::Clamp(FMath::CeilToInt(IslandHeight / CellSize), 1, TNumericLimits<int32>::Max());

	//initilize the grid with default values and a specific predetermined size
	GridPoints.Init(FVector2D::ZeroVector, GridWidth * GridHeight);
	GridBiomes.Init(-1, GridWidth * GridHeight);

	//UE_LOG(LogTemp, Warning, TEXT("Values:::: %i, %i, %f, %f, %i, %f"), GridHeight, GridWidth, IslandWidth, IslandHeight, GridPoints.Num(), CellSize)
		/*
			Add an inital first value to the grid
		*/
		//within bounds of island rectangle determine a random point
	float InitalXValue = IslandHeight / 2;//dRange(0.0f, IslandWidth / CellSize); 
	float InitalYValue = IslandWidth / 2; //FMath::RdandRange(0.0f, IslandHeight / CellSize);

	//determine the index of the point within the grid i.e the grid cell the point belongs too
	int32 XPosition = FMath::Clamp(FMath::FloorToInt(InitalXValue / CellSize), 0, GridWidth - 1);
	int32 YPosition = FMath::Clamp(FMath::FloorToInt(InitalYValue / CellSize), 0, GridHeight - 1);
	int32 InitialGridLocation = YPosition * GridWidth + XPosition;
	//add the inital point into the grid
	GridPoints[InitialGridLocation] = FVector2D(InitalXValue, InitalYValue);

	int32 NewBiome = DetermineBiome(GridBiomes[InitialGridLocation], DifferentBiomesMap, Stream);
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
	{//issue is multiple items can go into the same cell, overriding its current value and as result messing everything up
		//random active index of the grid
		int32 ActiveGridIndex = Stream.RandRange(0, ActiveList.Num() - 1);
		int32 ActiveGridIndexValue = ActiveList[ActiveGridIndex]; //get the value stored at this grid location
		FVector2D ActiveIndexLocation = GridPoints[ActiveGridIndexValue];

		//test upto k points until a valid one between Radius and 2*Radius of the ActivePoint is found or all k return invalid
		bool bValidCandidate = false;
		for (int32 a = 0; a < k; a++)
		{
			//generate a random point between r and 2r of the active one
			//determine location of new point to check
			float Angle = Stream.RandRange(0.0f, 1.0f) * PI * 2;
			//FVector Direction3D = UKismetMathLibrary::RandomUnitVector();//get direction to offset the point by
			FVector2D OffsetDirection = FVector2D(FMath::Cos(Angle), FMath::Sin(Angle));//FVector2D(Direction3D.X, Direction3D.Y); //get a random direction to offset the current active point by
			float OffsetDistance = 2 * Radius;// FMath::RdandRange(2 * Radius, Radius + 1); //get a random distance away from the current active point between Radius and 2 * Radius
			
			//determine the location and cell the offset point belongs too
			FVector2D OffsetPosition = ActiveIndexLocation + OffsetDirection * OffsetDistance; //the new point will be an offset of the active point based on above parameters
			int32 OffsetGridXPosition = FMath::Clamp(FMath::FloorToInt(OffsetPosition.X / CellSize), 0, GridWidth - 1); //the position of this item within the grid, clamped between the min and max size of the grid
			int32 OffsetGridYPosition = FMath::Clamp(FMath::FloorToInt(OffsetPosition.Y / CellSize), 0, GridHeight - 1);

			//due to the setup only need to check the neighbours grid cell one cell away from the current point
			bool bOffsetValid = true;
			for (int32 i = -1; i <= 1; i++)
			{
				for (int32 j = -1; j <= 1; j++)
				{
					//as long as the new point falls within the bounds of the island it can be used
					if (OffsetGridXPosition + j >= 0 && OffsetGridXPosition + j < GridWidth && OffsetGridYPosition + i >= 0 && OffsetGridYPosition + i < GridHeight)
					{
						int32 NeighbourGridIndex = (OffsetGridYPosition + i) * GridWidth + (OffsetGridXPosition + j);
						float Distance = FVector2D::Distance(OffsetPosition, GridPoints[NeighbourGridIndex]); //determine the distance between the active point and its neighbour's location
						if (GridBiomes[NeighbourGridIndex] != -1 && Distance < Radius) //if the offset point testing is too close to another already existing point
							bOffsetValid = false;
					}
					else //as outside array bounds it is also invalid
						bOffsetValid = false;
				}
			}

			//if the offseted point is valid then add it as a new point
			if (bOffsetValid)
			{
				int32 OffsetGridIndex = (OffsetGridYPosition) * GridWidth + (OffsetGridXPosition);
				GridPoints[OffsetGridIndex] = OffsetPosition;

				NewBiome = DetermineBiome(GridBiomes[ActiveGridIndexValue], DifferentBiomesMap, Stream);
				GridBiomes[OffsetGridIndex] = NewBiome;

				//add the point as a valid biome location
				BiomePoints.Add(TPair<int32, FVector2D>(NewBiome, FVector2D(OffsetPosition.X + XOriginOffset, OffsetPosition.Y + YOriginOffset)));
				ActiveList.Add(OffsetGridIndex); //add the index of the point to the active list

				bValidCandidate = true;
				break; //as valid point added, no need to do more searching with this point
			}

		}
		if (!bValidCandidate) //as no valid point was found, remove it from the list so no new points can spawn around it
			ActiveList.Remove(ActiveGridIndexValue); //entire issue here is that somehow multiple of the same location can be added to the list
	}

	return BiomePoints;
}

int32 PoissonDiskSampling::DetermineBiome(int32 NeighbourBiome, const TMap<int32, FBiomeStats>& DifferentBiomesMap, FRandomStream& Stream) //needs a reference to the biomes list
{
	if (NeighbourBiome == -1) //i.e no biome exists yet
	{
		int32 RandBiome = Stream.RandRange(7, 12); //the keys for the biomes not bound by specific conditions

		return RandBiome;
	}
	else
	{
		//pick random biome from list of all possible biomes
		int32 RandBiome = Stream.RandRange(0, DifferentBiomesMap[NeighbourBiome].NeighbourBiomeKeys.Num() - 1); //also tie in the rarity system somehow
		return DifferentBiomesMap[NeighbourBiome].NeighbourBiomeKeys[RandBiome]; //return the value stored at the randomly choosen position within the array
	}
}

////bool PoissonDiskSampling::bIsValid(FVector2D CandidatePoint, int32 GridPosition, int32 GridXSize, int32 CellSize)
////{
////	//int32 X = FMath::FloorToInt(GridPosition.X / CellSize) * GridXSize + FMath::FloorToInt(OffsetPoint.X / CellSize)
////	
////	return false;
////}
//
//	//grids width and height will be the islands width and height, determined when generating the islands
//TArray<FVector2D> Grid; //the list of points, being a 1D implementation of a 2D array
//TArray<bool> bGridHasValue; //the list of points just checking if it at the specified point contains a value or not
//int32 CellSize = Radius / FMath::Sqrt(2); //side of a gridcell is the radius / number of dimentions i.e 2
////actual size of a cell of the grid
//
//int32 GridXSize = FMath::FloorToInt(IslandWidth / CellSize); //the number of points in X direction for the grid
//int32 GridYSize = FMath::FloorToInt(IslandHeight / CellSize); //the number of points in Y direction for the grid
////1 or something will indicate that this cell does contain a value
//Grid.Init(FVector2D(0, 0), GridXSize* GridYSize); //create a grid of values initilized to 0(empty) so each point is Radius appart
//bGridHasValue.Init(false, GridXSize* GridYSize); //create a grid of values initilized to 0(empty) so each point is Radius appart
//
////randomly choose an inital starting point to be somehwhere on the grid
//int32 RandomPointX = FMath::d(0, IslandWidth - 1); //give a random point within bounds of the island
//int32 RandomPointY = FMath::dRandRange(0, IslandHeight - 1); //give a random point within bounds of the island
//int32 GridPoint = FMath::FloorToInt(RandomPointY / CellSize) * GridXSize + FMath::FloorToInt(RandomPointX / CellSize); //get the index on the grid this point is within
//Grid[GridPoint] = FVector2D(RandomPointX, RandomPointY);
//bGridHasValue[GridPoint] = true;
//BiomePoints.Add(TPair<int32, FVector2D>(5, FVector2D(RandomPointX, RandomPointY))); //add this + an offet to get the point centred around island not 0,0
//
////Queue is add from top and remove from the bottom
//TArray<int32> ActiveList; //list of all possible locations for points, holding the index of the GridArray
//ActiveList.Add(GridPoint);
//
//while (ActiveList.Num() > 0) //while elements exist within the ActiveList
//{
//	UE_LOG(LogTemp, Warning, TEXT("Truing to poisson disk sampling again"))
//		int32 RandomActivePoint = FMath::RdandRange(0, ActiveList.Num() - 1); //give the actual world co-ordinates of a point from the active list
//	FVector2D RandomPointPosition = Grid[RandomActivePoint]; //position of a random point on the grid
//
//	bool bValidCanditdate = false;
//	for (int32 a = 0; a < k; a++) //do until a valid point is found or the limit of max points to check has been reached
//	{
//		//randomly choose a point between r and 2r of the point choosen
//		//if its valid add it to the ActiveList and 
//		//then the point choosen from active list must be valid as well so add it to the list of biome points
//
//		//pick a random angle between 0 and 360
//		//pick random point between r and 2r
//		//offset the random point from the centre point by the angle and distance
//		////////////FMath::Sin
//
//		//determine location of new point to check
//		FVector Direction3D = UKismetMathLibrary::RandomUnitVector();//FVector2D::UnitVector;
//		FVector2D Direction2D = FVector2D(Direction3D.X, Direction3D.Y); //converts the 3D vector into a 2D vector
//		float Distance = FMath::RadndRange(Radius, 2 * Radius);
//		FVector2D OffsetPoint = RandomPointPosition + Distance * Direction2D; //gives us the points position offset by a direction for a distance
//		int32 YPos = FMath::Clamp(FMath::FloorToInt(OffsetPoint.Y / CellSize), 0, GridYSize - 1);
//		int32 XPos = FMath::Clamp(FMath::FloorToInt(OffsetPoint.X / CellSize), 0, GridXSize - 1);
//		int32 GridPosition = YPos * GridXSize + XPos; //the position the point will be on the grid
//
//
//		//check to ensure the new point is not too close to any existing point
//		bool bIsOk = true;
//		for (int32 i = -1; i <= 1; i++)
//		{
//			for (int32 j = -1; j <= 1; j++)
//			{
//				int32 CurrXPos = FMath::FloorToInt(RandomPointPosition.X / CellSize);
//				int32 CurrYPos = FMath::FloorToInt(RandomPointPosition.Y / CellSize);
//
//				if (CurrXPos + j >= 0 && CurrXPos + j < GridXSize && CurrYPos + i >= 0 && CurrYPos + i < GridXSize)///(CurrYPos + i) *GridXSize + (CurrXPos + j) >= 0 && (CurrYPos + i) * GridXSize + (CurrXPos + j) < GridXSize)
//				{
//					//calculate distance between new point and checking point
//					if (bGridHasValue[(CurrYPos + i) * GridXSize + (CurrXPos + j)]) //as long as neighbour cell has a point
//					{
//						float CheckDistance = FVector2D::Distance(Grid[(CurrYPos + i) * GridXSize + (CurrXPos + j)], OffsetPoint); //get distance between neighbour cell and current offset point
//						if (CheckDistance < Radius)//if the offet point is too close to another point 
//						{
//							//bValidCanditdate = false;
//							bIsOk = false; //point no longer ok/ valid
//							////break;
//						}
//					}
//				}
//			}
//		}
//		if (bIsOk) //if choosen point is not too close to any other nearby point
//		{
//			ActiveList.Add(GridPosition);
//			Grid[GridPosition] = OffsetPoint;
//			bGridHasValue[GridPosition] = true;
//			//biome list will add this real active point into it
//			BiomePoints.Add(TPair<int32, FVector2D>(4, OffsetPoint)); //add this + an offet to get the point centred around island not 0,0
//			bValidCanditdate = true;
//			break;//as point valid no need to add any more around it so break
//		}
//
//
//
//
//		////if (bIsValid(RandomPointPosition, GridPosition, GridXSize))//is any other point too close to this one
//		////{
//		////	ActiveList.Add(GridPosition);
//		////	Grid[GridPosition] = OffsetPoint;
//		////	//biome list will add this real active point into it
//		////	BiomePoints.Add(OffsetPoint); //add this + an offet to get the point centred around island not 0,0
//		////	bValidCanditdate = true;
//		////	break;//as point valid no need to add any more around it so break
//		////}
//		// //the actual position of the point on the grid
//
//		//for all neighbouring points on grid check them
//
//		//check the spot one to the left and right of the point on grid and 
//		//neighbour position = actual position of the neighbour's point
//		//distance between random position and neighbours position is < r  means it is too close to another point so discard it
//		//just add
//	}
//	//point must not be valid anymore so remove it as an active one
//	if (!bValidCanditdate)
//		ActiveList.RemoveAt(RandomActivePoint);
//}
//
////return a list of vector's containing all possible points
////as the array is in 1 by 1 units need to convert the vertor to be firstly its X and Y location in grid * CellSize +
////an X and Y offset to make the vector centred around the island instead of being around the world origin
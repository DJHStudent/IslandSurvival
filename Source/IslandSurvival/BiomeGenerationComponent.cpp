// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerationComponent.h"
//#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"

// Sets default values for this component's properties
UBiomeGenerationComponent::UBiomeGenerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	IslandKeys = 0;
	SingleIslandMaxSize = 2000;
	////DiskSampling = CreateDefaultSubobject<PoissonDiskSampling>("Sampling");
	// ...
}


// Called when the game starts
void UBiomeGenerationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBiomeGenerationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UBiomeGenerationComponent::AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition) //code to determine where each island is in the world
{
	int32 CurrentVertexPosition = YPosition * TerrainGenerator->Width + XPosition; //note this point is not yet added to the array
	if (ZPosition < WaterLine) //must be underwater so not an island
	{
		TerrainGenerator->IslandNumber.Add(-1); //-1 means underwater
		TerrainGenerator->VerticeColours[CurrentVertexPosition] = DifferentBiomesMap[1].BiomeColour; //as underwater set biome to ocean
		BiomeAtEachPoint[CurrentVertexPosition] = 1;
		//do nothing yet
	}
	else //must be above the water and as a result an island
	{
		//first check with all other terrain vertices around it to see what island they relate to
		int32 IslandPoint = -1; //the island point near this island
		if (XPosition - 1 >= 0) //note this method works as due to the falloff map the border vertices are 100% underwater anyway
		{
			//UE_LOG(LogTemp, Warning, TEXT("Current Element checking is 1: %i, %i, %i, YPosition is: %i, XPosition is: %i"), CurrentVertexPosition, YPosition * TerrainGenerator->Width + (XPosition - 1), TerrainGenerator->IslandNumber.Num(), YPosition, XPosition)
			int32 NewPoint = TerrainGenerator->IslandNumber[YPosition * TerrainGenerator->Width + (XPosition - 1)]; //get a node one behind the current node, if exists
			if (NewPoint != -1) //as long as the new point is not underwater, it must then be connected to this island
			{
				IslandPoint = NewPoint; //as first direction checking and only just found out it is its 
			}	//	else
			//ZPosition = 0;
			//UE_LOG(LogTemp, Warning, TEXT("Current Element checking is 1: %i"), CurrentVertexPosition)
		}

		//get node one up and back is j - i, i - 1
		if (XPosition - 1 >= 0 && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition - 1)]; //get a node one diagonally up and back from the current node, if exists
			if (NewPoint != -1)
			{
				//if it is next to a point which is 100% land, but a different island need to join the two
				if (IslandPoint != -1 && IslandPoint != NewPoint) //if not the first island point met and different
					JoinIslands(IslandPoint, NewPoint);
				else
					IslandPoint = NewPoint;
			}	//	else
			//ZPosition = 0;
			//UE_LOG(LogTemp, Warning, TEXT("Current Element checking is 2: %i"), CurrentVertexPosition)
		}

		//get node one up is i - 1
		if (YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + XPosition]; //get a node one up from the current node, if exists
			if (NewPoint != -1)
			{
				//if it is next to a point which is 100% land, but a different island need to join the two
				if (IslandPoint != -1 && IslandPoint != NewPoint) //if not the first island point met and different
					JoinIslands(IslandPoint, NewPoint);
				else
					IslandPoint = NewPoint;
			}//	else
			//ZPosition = 0;
			//UE_LOG(LogTemp, Warning, TEXT("Current Element checking is 3: %i"), CurrentVertexPosition)
		}
	
		//get node one up and forward is j + i, i - 1
		if (XPosition + 1 < TerrainGenerator->Width && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition + 1)]; //get a node one diagonally up and forward of the current node, if exists
			//new point is currently the key relating to the island looking for
			if (NewPoint != -1)
			{
				//if it is next to a point which is 100% land, but a different island need to join the two
				if (IslandPoint != -1 && IslandPoint != NewPoint) //if not the first island point met and different
					JoinIslands(IslandPoint, NewPoint);
				else
					IslandPoint = NewPoint;
				//UE_LOG(LogTemp, Warning, TEXT("Current Element checking is 4: %i"), CurrentVertexPosition)

			}	//	else
			//ZPosition = 0;
		}

		//once all points are checked can go through and actaully add the points to the nessesary array
		if (IslandPoint == -1) //as all points around it are underwater it must be an entirly new Island
		{
			FIslandStats IslandStats;
			IslandPointsMap.Add(IslandKeys, IslandStats);
			IslandPointsMap[IslandKeys].VertexIndices.Add(CurrentVertexPosition); //Num() -1 gets the last island added i.e the new island
			IslandPointsMap[IslandKeys].MinXPosition = XPosition;
			IslandPointsMap[IslandKeys].MaxXPosition = XPosition;
			IslandPointsMap[IslandKeys].MinYPosition = YPosition;
			IslandPointsMap[IslandKeys].MaxYPosition = YPosition;
			TerrainGenerator->IslandNumber.Add(IslandKeys);

			IslandKeys++;//as a new Island has been made add will need another Key for the next island
		}
		else //it is part of an existing island
		{
			IslandPointsMap[IslandPoint].VertexIndices.Add(CurrentVertexPosition);
			IslandPointsMap[IslandPoint].UpdateIslandBounds(FVector2D(XPosition, YPosition));
			TerrainGenerator->IslandNumber.Add(IslandPoint);
		}
	}
	return ZPosition;
}

void UBiomeGenerationComponent::JoinIslands(int32 IslandPoint, int32 NewPoint) //for 2 islands which are actually joined, add them together
{
	//island point is the current point at ...... NewPoint is the neighbouring point checking
	for (int32 i = 0; i < IslandPointsMap[NewPoint].VertexIndices.Num(); i++)// Point in IslandPointsMap[NewPoint])
	{
		IslandPointsMap[IslandPoint].VertexIndices.Add(IslandPointsMap[NewPoint].VertexIndices[i]);//Point);
		TerrainGenerator->IslandNumber[IslandPointsMap[NewPoint].VertexIndices[i]] = IslandPoint;

		//update the size of the island to reflect the new values if nessesary
		if (IslandPointsMap[NewPoint].MinXPosition < IslandPointsMap[IslandPoint].MinXPosition)
			IslandPointsMap[IslandPoint].MinXPosition = IslandPointsMap[NewPoint].MinXPosition;
		if (IslandPointsMap[NewPoint].MinYPosition < IslandPointsMap[IslandPoint].MinYPosition)
			IslandPointsMap[IslandPoint].MinYPosition = IslandPointsMap[NewPoint].MinYPosition;

		if (IslandPointsMap[NewPoint].MaxXPosition > IslandPointsMap[IslandPoint].MaxXPosition)
			IslandPointsMap[IslandPoint].MaxXPosition = IslandPointsMap[NewPoint].MaxXPosition;
		if (IslandPointsMap[NewPoint].MaxYPosition > IslandPointsMap[IslandPoint].MaxYPosition)
			IslandPointsMap[IslandPoint].MaxYPosition = IslandPointsMap[NewPoint].MaxYPosition;
	}
	IslandPointsMap.Remove(NewPoint); //remove the old island which is copied to the actual island list
}

//void UBiomeGenerationComponent::ColourOfIsland()
//{
//	//loop through the map and for each island give a colour to it
//	for (auto& Point : IslandPointsMap) 
//	{
//		FLinearColor RandColour = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));
//		//UE_LOG(LogTemp, Error, TEXT("Island Sizes Are: %i"), Point.Value.Num())
//		int32 NumBiomes = FMath::CeilToInt(Point.Value.Num() / 2000);
//		//NumBiomes = FMath::Clamp(NumBiomes, 1, 5);
//		TArray<TPair<int32, FVector2D>> BiomePositions; //the biome and its position
//		for (int32 j = 0; j < NumBiomes; j++) //for each island scatter a number of random points around map, being the biomes location
//		{
//			int32 position = FMath::RandRange(0, Point.Value.Num() - 1);
//			int32 RandomBiome = FMath::RandRange(3, 10);
//
//			FVector2D location = FVector2D(TerrainGenerator->Vertices[Point.Value[position]].X, TerrainGenerator->Vertices[Point.Value[position]].Y);
//			//UE_LOG(LogTemp, Error, TEXT("Adding Biome Point: %i, %s"), RandomBiome, *location.ToString())
//
//			BiomePositions.Add(TPair<int32, FVector2D>(RandomBiome, location));
//		}
//		for (int32 i = 0; i < Point.Value.Num(); i++)
//		{
//			int32 PointValue = Point.Value[i];
//			if (Point.Value.Num() <= 10) //biomes less than x size
//			{
//				TerrainGenerator->VerticeColours[PointValue] = DifferentBiomesMap[3].BiomeColour;
//				BiomeAtEachPoint[PointValue] = 3;
//			}
//			else if (Point.Value.Num() <= 50) //biomes less than x size
//			{
//				TerrainGenerator->VerticeColours[PointValue] = DifferentBiomesMap[2].BiomeColour;
//				BiomeAtEachPoint[PointValue] = 2;
//			}
//			else
//			{
//				int32 NearestBiome = 0;
//				float MinDist = TNumericLimits<float>::Max();
//				FVector2D currentLocation = FVector2D(TerrainGenerator->Vertices[PointValue].X, TerrainGenerator->Vertices[PointValue].Y);
//
//				for (int k = 0; k < BiomePositions.Num(); k++)
//				{
//					////UE_LOG(LogTemp, Error, TEXT("Set Biome As: %i"), NearestBiome)
//					float newDist = FVector2D::Distance(currentLocation, BiomePositions[k].Value);
//					if (newDist < MinDist) {
//						NearestBiome = BiomePositions[k].Key;
//						MinDist = newDist;
//					}
//				}
//				//UE_LOG(LogTemp, Error, TEXT("Adding Biome Point: %i, %i"), MinDist, BiomePositions.Num())
//				TerrainGenerator->VerticeColours[PointValue] = DifferentBiomesMap[NearestBiome].BiomeColour;
//				BiomeAtEachPoint[PointValue] = NearestBiome;
//			}
//		}
//	}
//	//UE_LOG(LogTemp, Error, TEXT("Number of Biomes, %i"), BiomePositions.Num())
//}

void UBiomeGenerationComponent::VerticesBiomes() //determine the biome for each vertex above waterline
{
	for (auto& IslandPair : IslandPointsMap)
	{
		int32 IslandSize = IslandPair.Value.VertexIndices.Num(); //the size or number of vertices which make up an island
		if (IslandSize < SingleIslandMaxSize)
			SingleBiomeIslands(IslandPair, IslandSize);
		else
			MultiBiomeIslands(IslandPair, IslandSize);
	}
}

void UBiomeGenerationComponent::SingleBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize)
{
	int32 RandomBiome = FMath::RandRange(3, 9); //from biome list pick random one
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value.VertexIndices) //for each point stored in the specific island
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed when determining biomes"))
		if (IslandSize <= 10) //make island a specific type(rocky outcrop)
		{
			UpdateBiomeLists(DifferentBiomesMap[3].BiomeColour, 3, VertexIdentifier);

			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 3)
		}
		else if (IslandSize <= 50) //make island a specific type(sandbar)
		{
			UpdateBiomeLists(DifferentBiomesMap[2].BiomeColour, 2, VertexIdentifier);

			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 2)
		}
		else //randomly choose a biome from the list
		{
			//FLinearColor RandColour = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));

			if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
			{
				UpdateBiomeLists(DifferentBiomesMap[RandomBiome].BiomeColour, RandomBiome, VertexIdentifier);
			}
			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, RandomBiome)
			//HeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier);
			//based on vertex height pick biome
		}
	}
}

void UBiomeGenerationComponent::MultiBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize)
{
	//use poisson disk sampling here to give a more even distribution of the biomes
	int32 NumBiomes = FMath::CeilToInt(IslandSize / SingleIslandMaxSize); //based on islands size the number of biomes which can spawn there

	//radius will be determined based on SingleIslandMaxSize, k is determined via testing
	//biomes sizes will need to be * by grid size to get actual positions for them

	//get the straight line width of the island * by grid size so it gets the actual width not the distance between the two positions in array
	float IslandWidth = (IslandVertexIdentifiers.Value.MaxXPosition - IslandVertexIdentifiers.Value.MinXPosition) * TerrainGenerator->GridSize;
	float IslandHeight = (IslandVertexIdentifiers.Value.MaxYPosition - IslandVertexIdentifiers.Value.MinYPosition) * TerrainGenerator->GridSize;
	float Radius = TerrainGenerator->Width * TerrainGenerator->Height / 10; //as size doubles radius shouldn't double //come back too later
	TArray<TPair<int32, FVector2D>> BiomePositions = DiskSampling.CreatePoints(Radius, 30, IslandWidth, IslandHeight, IslandVertexIdentifiers.Value.MinXPosition * TerrainGenerator->GridSize, IslandVertexIdentifiers.Value.MinYPosition * TerrainGenerator->GridSize);
	for (int32 pp = 0; pp < BiomePositions.Num(); pp++)
	{
		UE_LOG(LogTemp, Error, TEXT("Number Of Biomes: %s, %i"), *BiomePositions[pp].Value.ToString(), BiomePositions[pp].Key)
	}
	//for (int32 j = 0; j < NumBiomes; j++) //for each island scatter a number of random points around map, being the biomes location
	//{ //possible for two biome to share the same point with this method
	//	int32 position = FMath::RandRange(0, IslandSize - 1); //pick random array element for island to spawn at
	//	int32 RandomBiome = FMath::RandRange(3, 9); //from biome list pick random one

	//	//get the location of the location of the point choosen from the island list of points
	//	FVector2D location = FVector2D(TerrainGenerator->Vertices[IslandVertexIdentifiers.Value[position]].X, TerrainGenerator->Vertices[IslandVertexIdentifiers.Value[position]].Y);
	//	BiomePositions.Add(TPair<int32, FVector2D>(RandomBiome, location)); //add the choosen biome and position to the list
	//}


	//for each vertex determine closest biome point near
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value.VertexIndices) //for each point stored in the specific island
	{
		int32 NearestBiome = 0; //biome point will be, currently ocean
		int32 SecondNearestBiome = 0;
		float MinDist = TNumericLimits<float>::Max(); //distance to the nearest point a biome can be at
		float MinDist2 = TNumericLimits<float>::Max(); //distance to the second nearest point a biome can be at
		FVector2D currentLocation = FVector2D(TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y); //location of the current vertex in world

		for (int k = 0; k < BiomePositions.Num(); k++) //determine the biome point nearest
		{
			////UE_LOG(LogTemp, Error, TEXT("Set Biome As: %i"), NearestBiome)
			float CurrentDistance = FVector2D::Distance(currentLocation, BiomePositions[k].Value); //for all biome points distance too it
			if (k == 1) //issue if k1 is not nearer it will fail
			{
				if (CurrentDistance < MinDist) { //as the new point is a closer biome use this one
					MinDist2 = MinDist; //as only second point first point i.e k = 0 would be the second closest
					SecondNearestBiome = NearestBiome;
				}
				else //min biome closest is still the closest so set the current one to the biome
				{
					SecondNearestBiome = BiomePositions[k].Key;
					MinDist2 = CurrentDistance;
				}
			}
			else if (CurrentDistance > MinDist && CurrentDistance < MinDist2)
			{
				MinDist2 = CurrentDistance; //second closest point
				SecondNearestBiome = BiomePositions[k].Key;
				//each point has one biome closest to
				//point second closest to would be min distance
				//Current Distance > min distance but less than MinDist2
			}
			if (CurrentDistance < MinDist) //find the biome point closest too this one
			{
				NearestBiome = BiomePositions[k].Key;
				MinDist = CurrentDistance;
			}
		}
		//assign the biome to the point
		if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
		{
			////distance to second nearest point - distance to nearest point = distance to edge
			//float EdgeDistance = MinDist2 - MinDist; //gives the distance from the point to the biomes border edge
			//float TotalDistance = EdgeDistance + MinDist; //gives us the total distance from centre, through point to edge
			//float DistPercent = MinDist / TotalDistance; //gives % of way point is along path 100% means at edge 0% means at centre
			FLinearColor NearestBiomeColour = DifferentBiomesMap[NearestBiome].BiomeColour;
			//FLinearColor SecondNearestBiomeColour = DifferentBiomesMap[SecondNearestBiome].BiomeColour;
			//UE_LOG(LogTemp, Warning, TEXT("Biomes closest to point are: % i, % i"), NearestBiome, SecondNearestBiome)
			FLinearColor PointColour;
			//if (DistPercent > .9f)
			//{ //or check all 8 tiles around current one and if not same biome
			//	//normalise the point to be between a new range
			//	DistPercent = ((DistPercent - 0.9f) / (1 - 0.9f)) * (1 - 0) + 0.0f;
			//	DistPercent = FMath::Clamp(DistPercent, 0.0f, 0.5f);
			//	PointColour = FMath::Lerp(NearestBiomeColour, SecondNearestBiomeColour, DistPercent); //a colour which is a blend between the two, bigger dist percent means more A
			//}
			//else
			PointColour = NearestBiomeColour;
			UpdateBiomeLists(PointColour, NearestBiome, VertexIdentifier); //as have nearest and second nearest biomes, just blend their colours together based on %distance to the border
			/*
				in theory a distance of halfway between the two biomes will be the border
				its X dist from one biome and X2 Dist from another

				need to find the edge location point where alfway between the two biomes
				then just check how far away from that point this one is in % and apply appropriate blendings
			*/
		}
		//determine if it should be a different biome based on height
		//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, NearestBiome)

		//HeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier);
	}
}

bool UBiomeGenerationComponent::bHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier)
{
	if (ZHeight > 700) //Testing of values until right one found
	{
		UpdateBiomeLists(DifferentBiomesMap[10].BiomeColour, 10, VertexIdentifier);
		return true;
		//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 10)
	}
	return false;
}

void UBiomeGenerationComponent::UpdateBiomeLists(FLinearColor BiomeColour, int32 Biome, int32 VertexIdentifier)
{
	//apply the appropriate colour to the terrain
	//just do the transition junk here
	//get the x and y position of point on map
	int32 XPosition = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].X / TerrainGenerator->GridSize);
	int32 YPosition = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].Y / TerrainGenerator->GridSize);
	XPosition = FMath::Clamp(XPosition, 0, TerrainGenerator->Width - 1);
	YPosition = FMath::Clamp(YPosition, 0, TerrainGenerator->Height - 1);
	
	TerrainGenerator->VerticeColours[VertexIdentifier] = BiomeColour; //for the vertex colour at the same position as it is in the verties array, give a colour
	BiomeAtEachPoint[VertexIdentifier] = Biome; //give each vertex the appropriate biome designation

	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list for each biome's positions on the map if it exists
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}
}

//void UBiomeGenerationComponent::BiomeBlending()
//{
//	for (int32 i = 0; i < TerrainGenerator->Height; i++)
//	{
//		for (int32 j = 0; j < TerrainGenerator->Width; j++)
//		{
//			int32 XPosition = j;
//			int32 YPosition = i;
//			int32 VertexIdentifier = YPosition * TerrainGenerator->Width + XPosition;
//			int32 Biome = BiomeAtEachPoint[VertexIdentifier];
//			//2 away would be 
//			int32 Radius = 1; //the range of values to go back to
//			int32 XStraightLineNum = 1;
//			int32 YStraightLineNum = 1;
//			float SmoothStep = .25f;
//			for (int32 k = 0; k < Radius; k++)
//			{
//
//				FLinearColor LerpedColour = DifferentBiomesMap[Biome].BiomeColour;
//				FLinearColor Colour = FLinearColor(0.6f, 0.6f, 0.6f);
//				////x + 1 y
//				if (XPosition + XStraightLineNum < TerrainGenerator->Width && BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition + XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition + XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition + XStraightLineNum)]].BiomeColour, SmoothStep);
//				////x - 1 y//
//				else if (XPosition - XStraightLineNum >= 0 && BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition - XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition - XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition)*TerrainGenerator->Width + (XPosition - XStraightLineNum)]].BiomeColour, SmoothStep);
//				//x y + 1
//				else if (YPosition + YStraightLineNum < TerrainGenerator->Height && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition)] != Biome && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition)]].BiomeColour, SmoothStep);
//				//x y - 1//
//				else if (YPosition - YStraightLineNum >= 0 && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition)] != Biome && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition)]].BiomeColour, SmoothStep);
//
//				//x + 1 y + 1
//				else if (XPosition + XStraightLineNum < TerrainGenerator->Width && YPosition + YStraightLineNum < TerrainGenerator->Height && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)]].BiomeColour, SmoothStep);
//				//x + 1y - 1//
//				else if (XPosition + XStraightLineNum < TerrainGenerator->Width && YPosition - YStraightLineNum >= 0 && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition + XStraightLineNum)]].BiomeColour, SmoothStep);
//				////x - 1 y - 1//
//				else if (XPosition - XStraightLineNum >= 0 && YPosition - YStraightLineNum >= 0 && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition - YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)]].BiomeColour, SmoothStep);
//				//////////x - 1 y + 1
//				else if (XPosition - XStraightLineNum >= 0 && YPosition + YStraightLineNum < TerrainGenerator->Height && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)] != Biome && BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)] != 1)
//					LerpedColour = Colour;//FMath::Lerp(LerpedColour, DifferentBiomesMap[BiomeAtEachPoint[(YPosition + YStraightLineNum) * TerrainGenerator->Width + (XPosition - XStraightLineNum)]].BiomeColour, SmoothStep);
//
//				TerrainGenerator->VerticeColours[VertexIdentifier] = LerpedColour; //for the vertex colour at the same position as it is in the verties array, give a colour
//				XStraightLineNum++;
//				YStraightLineNum++;
//				SmoothStep *= 0.5f;
//			}
//		}
//	}
//	/*
//		going up would be y - 1 -2 etc
//		same for down / left / right
//	*/
//}

void UBiomeGenerationComponent::SpawnMeshes() //spawn in the meshes into the map
{
	for (auto& BiomePoints : VertexBiomeLocationsMap) //for each biome on the map
	{
		int32 BiomeAmount = BiomePoints.Value.Num(); //number of points which make up the biome
		for (FBiomeMeshes DifferentMeshes : DifferentBiomesMap[BiomePoints.Key].BiomeMeshes) //for each mesh which can spawn in each biome on the map
		{
			//calculate the density of each point based on total points for the biome
			int32 MeshesDensity = FMath::CeilToInt(DifferentMeshes.Density / 100 * BiomeAmount);
			for (size_t i = 0; i < DifferentMeshes.Density; i++) //spawn in X meshes based on its density within the biome
			{
				//pick a random location within the specified biome
				int32 RandomLocation = FMath::RandRange(0, BiomePoints.Value.Num() - 1);
				int32 VertexIndex = BiomePoints.Value[RandomLocation];
				FVector VertexLocation = TerrainGenerator->Vertices[VertexIndex];

				FRotator Rotation = FRotator(0, 0, 0); //give the mesh a random Yaw rotation
				Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);

				//spawn in a new mesh in specified location, with rotation
				AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), VertexLocation, Rotation);
				SpawnedMesh->SetActorScale3D(FVector(FMath::RandRange(15.0f, 45.0f))); //give the mesh the same random scale on all 3 axis
				SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(DifferentMeshes.Mesh); //assign the appropriate mesh to the spawned in one

				//remove the choosen location from the list so no new meshes can spawn there
				BiomePoints.Value.RemoveAt(RandomLocation);
			}
		}
	}


	//if i have a list of all points within a biome then its easy


	//FVector VertexLocation = TerrainGenerator->Vertices[VertexIdentifier];

	//float XPosition = VertexLocation.X;//FMath::RandRange(VertexLocation.X - TerrainGenerator->GridSize / 2, VertexLocation.X + TerrainGenerator->GridSize / 2);
	//float YPosition = VertexLocation.Y;//FMath::RandRange(VertexLocation.Y - TerrainGenerator->GridSize / 2, VertexLocation.Y + TerrainGenerator->GridSize / 2);
	//FVector Location = FVector(XPosition, YPosition, VertexLocation.Z);

	//FRotator Rotation = FRotator(0,0, 0);
	//Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	//AStaticMeshActor* NewMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),Location, Rotation);
	//NewMesh->SetActorScale3D(FVector(FMath::RandRange(15.0f, 45.0f)));
	//NewMesh->GetStaticMeshComponent()->SetStaticMesh(DifferentBiomesMap[Biome].BiomeMeshes[0].Mesh);
}

//interesting seeds
/*
	3541
	30490
	10970
	11646
	1179
	32103(real interesting)
	7127(for sandbar/outcrop biomes)

	issue with the way generating the terrain is that really dependent upon the values you give it so for biomes not want that
	when talking about method used to do terrain mention this issue and how I did it through much trial and error to find best values
*/
// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerationComponent.h"

// Sets default values for this component's properties
UBiomeGenerationComponent::UBiomeGenerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	IslandKeys = 0;
	SingleIslandMaxSize = 2000;
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
			IslandPointsMap.Add(IslandKeys, TArray<int32>());
			IslandPointsMap[IslandKeys].Add(CurrentVertexPosition); //Num() -1 gets the last island added i.e the new island
			TerrainGenerator->IslandNumber.Add(IslandKeys);

			IslandKeys++;//as a new Island has been made add will need another Key for the next island
		}
		else //it is part of an existing island 
		{
			IslandPointsMap[IslandPoint].Add(CurrentVertexPosition);
			TerrainGenerator->IslandNumber.Add(IslandPoint);
		}
	}
	return ZPosition;
}

void UBiomeGenerationComponent::JoinIslands(int32 IslandPoint, int32 NewPoint) //for 2 islands which are actually joined, add them together
{
	//island point is the current point at ...... NewPoint is the neighbouring point checking
	for (int32 i = 0; i < IslandPointsMap[NewPoint].Num(); i++)// Point in IslandPointsMap[NewPoint])
	{
		IslandPointsMap[IslandPoint].Add(IslandPointsMap[NewPoint][i]);//Point);
		TerrainGenerator->IslandNumber[IslandPointsMap[NewPoint][i]] = IslandPoint;
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

void UBiomeGenerationComponent::VerticesBiomes()
{
	for (auto& IslandPair : IslandPointsMap)
	{
		int32 IslandSize = IslandPair.Value.Num(); //the size or number of vertices which make up an island
		if (IslandSize < 2000)
			SingleBiomeIslands(IslandPair, IslandSize);
		//else
			//MultiBiomeIslands();
	}
}

void UBiomeGenerationComponent::SingleBiomeIslands(TPair<int32, TArray<int32>> IslandVertexIdentifiers, int32 IslandSize)
{
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value) //for each point stored in the specific island
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed when determining biomes"))
		if (IslandSize <= 10) //make island a specific type(rocky outcrop)
		{
			TerrainGenerator->VerticeColours[VertexIdentifier] = DifferentBiomesMap[3].BiomeColour; //for the vertex colour at the same position as it is in the verties array, give a colour
		}
		else if (IslandSize <= 50) //make island a specific type(sandbar)
		{
			TerrainGenerator->VerticeColours[VertexIdentifier] = DifferentBiomesMap[2].BiomeColour; //for the vertex colour at the same position as it is in the verties array, give a colour
		}
		else //randomly choose a biome from the list
		{
			FLinearColor RandColour = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));
			TerrainGenerator->VerticeColours[VertexIdentifier] = RandColour;
			//based on vertex height pick biome
		}
	}
}
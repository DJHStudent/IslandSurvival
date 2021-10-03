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
		if (IslandSize < SingleIslandMaxSize)
			SingleBiomeIslands(IslandPair, IslandSize);
		else
			MultiBiomeIslands(IslandPair, IslandSize);
	}
}

void UBiomeGenerationComponent::SingleBiomeIslands(TPair<int32, TArray<int32>> IslandVertexIdentifiers, int32 IslandSize)
{
	int32 RandomBiome = FMath::RandRange(3, 9); //from biome list pick random one
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value) //for each point stored in the specific island
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed when determining biomes"))
		if (IslandSize <= 10) //make island a specific type(rocky outcrop)
		{
			UpdateBiomeLists(3, VertexIdentifier);

			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 3)
		}
		else if (IslandSize <= 50) //make island a specific type(sandbar)
		{
			UpdateBiomeLists(2, VertexIdentifier);

			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 2)
		}
		else //randomly choose a biome from the list
		{
			//FLinearColor RandColour = FLinearColor(FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.0f, 1.0f));

			if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
			{
				UpdateBiomeLists(RandomBiome, VertexIdentifier);

				if (RandomBiome == 8) //i.e a desert biome
				{
					SpawnMeshes(8, VertexIdentifier);
				}
			}
			//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, RandomBiome)
			//HeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier);
			//based on vertex height pick biome
		}
	}
}

void UBiomeGenerationComponent::MultiBiomeIslands(TPair<int32, TArray<int32>> IslandVertexIdentifiers, int32 IslandSize)
{
	int32 NumBiomes = FMath::CeilToInt(IslandSize / SingleIslandMaxSize); //based on islands size the number of biomes which can spawn there
	TArray<TPair<int32, FVector2D>> BiomePositions;
	for (int32 j = 0; j < NumBiomes; j++) //for each island scatter a number of random points around map, being the biomes location
	{
		int32 position = FMath::RandRange(0, IslandSize - 1); //pick random array element for island to spawn at
		int32 RandomBiome = FMath::RandRange(3, 9); //from biome list pick random one

		//get the location of the location of the point choosen from the island list of points
		FVector2D location = FVector2D(TerrainGenerator->Vertices[IslandVertexIdentifiers.Value[position]].X, TerrainGenerator->Vertices[IslandVertexIdentifiers.Value[position]].Y);
		BiomePositions.Add(TPair<int32, FVector2D>(RandomBiome, location)); //add the choosen biome and position to the list
	}


	//for each vertex determine closest biome point near
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value) //for each point stored in the specific island
	{
		int32 NearestBiome = 0; //biome point will be, currently ocean
		float MinDist = TNumericLimits<float>::Max(); //distance to the nearest point a biome can be at
		FVector2D currentLocation = FVector2D(TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y); //location of the current vertex in world

		for (int k = 0; k < BiomePositions.Num(); k++) //determine the biome point nearest
		{
			////UE_LOG(LogTemp, Error, TEXT("Set Biome As: %i"), NearestBiome)
			float newDist = FVector2D::Distance(currentLocation, BiomePositions[k].Value);
			if (newDist < MinDist) {
				NearestBiome = BiomePositions[k].Key;
				MinDist = newDist;
			}
		}
		//assign the biome to the point
		if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, 0, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
		{
			UpdateBiomeLists(NearestBiome, VertexIdentifier);


			if (NearestBiome == 8) //i.e a desert biome
			{
				SpawnMeshes(8, VertexIdentifier);
			}
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
		UpdateBiomeLists(10, VertexIdentifier);
		return true;
		//UE_LOG(LogTemp, Warning, TEXT("Current Pos and Biome: %f, %f, %i"), TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y, 10)
	}
	return false;
}

void UBiomeGenerationComponent::UpdateBiomeLists(int32 Biome, int32 VertexIdentifier)
{
	//apply the appropriate colour to the terrain
	TerrainGenerator->VerticeColours[VertexIdentifier] = DifferentBiomesMap[Biome].BiomeColour; //for the vertex colour at the same position as it is in the verties array, give a colour
	BiomeAtEachPoint[VertexIdentifier] = Biome; //give each vertex the appropriate biome designation

	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list for each biome's positions on the map if it exists
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}
}


void UBiomeGenerationComponent::SpawnMeshes(int32 Biome, int32 VertexIdentifier) //spawn in the meshes into the map
{
	FVector VertexLocation = TerrainGenerator->Vertices[VertexIdentifier];

	float XPosition = VertexLocation.X;//FMath::RandRange(VertexLocation.X - TerrainGenerator->GridSize / 2, VertexLocation.X + TerrainGenerator->GridSize / 2);
	float YPosition = VertexLocation.Y;//FMath::RandRange(VertexLocation.Y - TerrainGenerator->GridSize / 2, VertexLocation.Y + TerrainGenerator->GridSize / 2);
	FVector Location = FVector(XPosition, YPosition, VertexLocation.Z);

	FRotator Rotation = FRotator(0,0, 0);
	Rotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	AStaticMeshActor* NewMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(),Location, Rotation);
	NewMesh->SetActorScale3D(FVector(FMath::RandRange(15.0f, 45.0f)));
	NewMesh->GetStaticMeshComponent()->SetStaticMesh(DifferentBiomesMap[Biome].BiomeMeshes[0].Mesh);
}

//interesting seeds
/*
	3541
	30490
	10970
	11646
	1179
	32103(real interesting)

	issue with the way generating the terrain is that really dependent upon the values you give it so for biomes not want that
	when talking about method used to do terrain mention this issue and how I did it through much trial and error to find best values
*/
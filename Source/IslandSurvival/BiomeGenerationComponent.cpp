// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerationComponent.h"
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

void UBiomeGenerationComponent::AddIslandPoint(int32 XPosition, int32 YPosition, float ZPosition) //code to determine where each island is in the world
{
	int32 CurrentVertexPosition = YPosition * TerrainGenerator->Width + XPosition; //the position of the vertex within the array
	if (ZPosition < WaterLine) //must be underwater so not an island
	{
		TerrainGenerator->IslandNumber.Add(-1); //-1 means underwater and as a result do not need to check again
		TerrainGenerator->VerticeColours[CurrentVertexPosition] = DifferentBiomesMap[1].BiomeColour; //as underwater set biome to ocean
		BiomeAtEachPoint[CurrentVertexPosition] = 1; //the current biome of the vertex is ocean
	}
	else //must be above the water and as a result an island
	{
		//first check with all other terrain vertices around it to see what island they relate to
		int32 IslandPoint = -1; //the current island key the point is related to
		if (XPosition - 1 >= 0) //note this method works as due to the falloff map the border vertices are 100% underwater anyway
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[YPosition * TerrainGenerator->Width + (XPosition - 1)]; //get a vertex one behind, if exists and determine its island number
			if (NewPoint != -1) //as long as the new point is not underwater, it must then be connected to this island
				IslandPoint = NewPoint; //as first direction checking unknown if any other islands are yet nearby
		}

		//get node one up and back is j - i, i - 1
		if (XPosition - 1 >= 0 && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition - 1)]; //get a vertex diagonaly one behind and up, if exists and determine its island number
			if (NewPoint != -1)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (IslandPoint != -1 && IslandPoint != NewPoint)
					JoinIslands(IslandPoint, NewPoint); //for both elements of the IslandPointsMap join them together
				else //must be appart of the same island
					IslandPoint = NewPoint;
			}
		}

		//get node one up is i - 1
		if (YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + XPosition]; //get a vertex one up, if exists and determine its island number
			if (NewPoint != -1)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (IslandPoint != -1 && IslandPoint != NewPoint)
					JoinIslands(IslandPoint, NewPoint);
				else //must be appart of the same island
					IslandPoint = NewPoint;
			}//	else
		}
	
		//get node one up and forward is j + i, i - 1
		if (XPosition + 1 < TerrainGenerator->Width && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->IslandNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition + 1)]; //get a vertex one diagonally up and forward, if exists and determine its island number
			//new point is currently the key relating to the island looking for
			if (NewPoint != -1)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (IslandPoint != -1 && IslandPoint != NewPoint)
					JoinIslands(IslandPoint, NewPoint);
				else //must be appart of the same island
					IslandPoint = NewPoint;

			}
		}

		//once all 4 currently existing neighboruing points are checked add the point into the islands map
		if (IslandPoint == -1) //as all points around it are underwater it must be an entirly new Island
		{
			FIslandStats IslandStats;
			IslandPointsMap.Add(IslandKeys, IslandStats); //add the point as a new element
			IslandPointsMap[IslandKeys].VertexIndices.Add(CurrentVertexPosition); //add the vertex's position within the vertices array

			//update the min and max values for the island with the inital starting values
			IslandPointsMap[IslandKeys].MinXPosition = XPosition;
			IslandPointsMap[IslandKeys].MaxXPosition = XPosition;
			IslandPointsMap[IslandKeys].MinYPosition = YPosition;
			IslandPointsMap[IslandKeys].MaxYPosition = YPosition;
			TerrainGenerator->IslandNumber.Add(IslandKeys); //for the new vertex adding add the islands point to it

			IslandKeys++;//as a new Island has been made add will need another new Key for the next island
		}
		else //it is part of an existing island so add the point to the existing island
		{
			IslandPointsMap[IslandPoint].VertexIndices.Add(CurrentVertexPosition);
			IslandPointsMap[IslandPoint].UpdateIslandBounds(FVector2D(XPosition, YPosition)); //check to see if the islands bounds need to be updated
			TerrainGenerator->IslandNumber.Add(IslandPoint);
		}
	}
}

void UBiomeGenerationComponent::JoinIslands(int32 IslandPoint, int32 NewPoint) //for 2 islands which are actually one in the list, add them together
{
	for (int32 i = 0; i < IslandPointsMap[NewPoint].VertexIndices.Num(); i++) //for each vertex index stored in the new point key move to the Island point key
	{
		IslandPointsMap[IslandPoint].VertexIndices.Add(IslandPointsMap[NewPoint].VertexIndices[i]);
		TerrainGenerator->IslandNumber[IslandPointsMap[NewPoint].VertexIndices[i]] = IslandPoint;
	}

	//update the min and max positions of the island to reflect the new values if nessesary to do so
	if (IslandPointsMap[NewPoint].MinXPosition < IslandPointsMap[IslandPoint].MinXPosition)
		IslandPointsMap[IslandPoint].MinXPosition = IslandPointsMap[NewPoint].MinXPosition;
	if (IslandPointsMap[NewPoint].MinYPosition < IslandPointsMap[IslandPoint].MinYPosition)
		IslandPointsMap[IslandPoint].MinYPosition = IslandPointsMap[NewPoint].MinYPosition;

	if (IslandPointsMap[NewPoint].MaxXPosition > IslandPointsMap[IslandPoint].MaxXPosition)
		IslandPointsMap[IslandPoint].MaxXPosition = IslandPointsMap[NewPoint].MaxXPosition;
	if (IslandPointsMap[NewPoint].MaxYPosition > IslandPointsMap[IslandPoint].MaxYPosition)
		IslandPointsMap[IslandPoint].MaxYPosition = IslandPointsMap[NewPoint].MaxYPosition;

	IslandPointsMap.Remove(NewPoint); //remove the new island which is copied to the actual island list
}

void UBiomeGenerationComponent::VerticesBiomes() //determine the biome for each vertex above waterline
{
	for (auto& IslandPair : IslandPointsMap) //loop through each island which has been previously found
	{
		//determine the width and height of the island
		float IslandWidth = (IslandPair.Value.MaxXPosition - IslandPair.Value.MinXPosition); 
		float IslandHeight = (IslandPair.Value.MaxYPosition - IslandPair.Value.MinYPosition);

		int32 IslandSize = FMath::CeilToInt(IslandWidth * IslandHeight);//rectangular size of the island
		if (IslandSize < SingleIslandMaxSize) //if the island is small it will only have a single biome on it
			SingleBiomeIslands(IslandPair, IslandSize);
		else
			MultiBiomeIslands(IslandPair, IslandSize);
	}
}

void UBiomeGenerationComponent::SingleBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize)
{
	int32 RandomBiome = TerrainGenerator->Stream.RandRange(7, 12); //from biome list pick a random one which is also an above water, land(not mountain) biome
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value.VertexIndices) //for each vertex stored in the specific island
	{
		if (IslandSize <= 10) //make island a specific type(rocky outcrop)
			UpdateBiomeLists(2, VertexIdentifier);
		
		else if (IslandSize <= 50) //make island a specific type(sandbar)
			UpdateBiomeLists(3, VertexIdentifier);

		else //use the randomly choosen biome as the island's biome
		{
			if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, RandomBiome, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
				UpdateBiomeLists(RandomBiome, VertexIdentifier);
		}
	}
}

void UBiomeGenerationComponent::MultiBiomeIslands(TPair<int32, FIslandStats> IslandVertexIdentifiers, int32 IslandSize)
{
	//for these islands using voronoi noise with an even distribution of the points by poisson disk sampling to determine the locations of the biomes
	//this will create islands with biomes which have an even distribution

	float IslandWidths = (IslandVertexIdentifiers.Value.MaxXPosition - IslandVertexIdentifiers.Value.MinXPosition);
	float IslandHeights = (IslandVertexIdentifiers.Value.MaxYPosition - IslandVertexIdentifiers.Value.MinYPosition);

	//determine the actual size of the rectangular grid covering the island by using its min and max position * by grid size so its their actual real size
	float IslandWidth = (IslandVertexIdentifiers.Value.MaxXPosition - IslandVertexIdentifiers.Value.MinXPosition) * TerrainGenerator->GridSize;
	float IslandHeight = (IslandVertexIdentifiers.Value.MaxYPosition - IslandVertexIdentifiers.Value.MinYPosition) * TerrainGenerator->GridSize;
	
	//use poisson disk sampling here to give a more even distribution of the biomes
	TArray<TPair<int32, FVector2D>> BiomePositions = DiskSampling.CreatePoints(SingleIslandMaxSize, 30, IslandWidth, IslandHeight, IslandVertexIdentifiers.Value.MinXPosition * TerrainGenerator->GridSize, IslandVertexIdentifiers.Value.MinYPosition * TerrainGenerator->GridSize, DifferentBiomesMap, TerrainGenerator->Stream);


	//using a voronoi noise method which for each vertice just determine the biome point it is nearest
	for (int32 VertexIdentifier : IslandVertexIdentifiers.Value.VertexIndices) //for each point stored in the specific island
	{
		int32 NearestBiome = 1; //biome the vertex will be
		float MinDist = TNumericLimits<float>::Max(); //the distance to the current closest biome point
		FVector2D currentLocation = FVector2D(TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y); //location of the current vertex in world

		for (int k = 0; k < BiomePositions.Num(); k++) //for each biome which will exist on the island determine the nearest one
		{
			float CurrentDistance = FVector2D::Distance(currentLocation, BiomePositions[k].Value); //the distance to the current point checking
			if (CurrentDistance < MinDist) //as closer than previous point it will be the new biome of the vertex
			{
				NearestBiome = BiomePositions[k].Key; //update with the new biome of the point
				MinDist = CurrentDistance;
			}
		}
		//as closest biome found update vertex with its biome
		if (!bHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, NearestBiome, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
			UpdateBiomeLists(NearestBiome, VertexIdentifier); //use the non-height based biome to update
	}
}

bool UBiomeGenerationComponent::bHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier)
{
	if (ZHeight > 700) //check if the Z position of the point is above the specified value
	{
		//two heigh biomes have keys of 5 and 6 respectivly
		for (int32 NeighbourBiome : DifferentBiomesMap[5].NeighbourBiomeKeys) //check the possible neighbour biomes for 5 first
		{
			if (NeighbourBiome == Biome) //if the lower elevation biome is a neighbour then use 5 to update the list
			{
				UpdateBiomeLists(5, VertexIdentifier);
				return true; //as biome found return true
			}
		}

		//just do same again but as not biome 5 test it with biome 6
		for (int32 NeighbourBiome : DifferentBiomesMap[6].NeighbourBiomeKeys)
		{
			if (NeighbourBiome == Biome)
			{
				UpdateBiomeLists(6, VertexIdentifier);
				return true; 
			}
		}
	}
	return false; //current height not high enough for it to be a height biome
}

void UBiomeGenerationComponent::UpdateBiomeLists(int32 Biome, int32 VertexIdentifier)
{	

	TerrainGenerator->VerticeColours[VertexIdentifier] = DifferentBiomesMap[Biome].BiomeColour; //for the specified biome assign the vertex the appropriate colour
	BiomeAtEachPoint[VertexIdentifier] = Biome; //also give each vertex the appropriate biome

	//assign the appropriate height value to the vertex
	if (Biome == 9 || Biome == 12 || Biome == 11 || Biome == 8 || Biome == 10)// || Biome == 12)
	{
		//DifferentBiomesMap[Biome].BiomeHeight.TerraceSize = TerrainGenerator->TerraceSize;
		DifferentBiomesMap[Biome].BiomeHeight.OcataveOffsets = TerrainGenerator->OcataveOffsets;

		int32 XPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].X / TerrainGenerator->GridSize);
		int32 YPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].Y / TerrainGenerator->GridSize);
		TerrainGenerator->Vertices[VertexIdentifier].Z = DifferentBiomesMap[Biome].BiomeHeight.GenerateHeight(XPos, YPos);
	}


	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list holding each biome and the vertices contained within them, if they exist
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list as it has not been seen before
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}
}

void UBiomeGenerationComponent::SpawnMeshes() //spawn in the meshes into the map
{
	for (auto& BiomePoints : VertexBiomeLocationsMap) //for each biome on the map
	{
		if (DifferentBiomesMap[BiomePoints.Key].BiomeMeshes.Num() > 0) //only do as long as biome contains meshes to be spawned in
		{
			int32 BiomeAmount = BiomePoints.Value.Num(); //number of points which make up the biome
			for (FBiomeMeshes DifferentMeshes : DifferentBiomesMap[BiomePoints.Key].BiomeMeshes) //for each mesh which can spawn in at the current biome
			{
				//calculate number of each mesh to spawn in based on its % density of total points
				int32 MeshesDensity = FMath::CeilToInt(DifferentMeshes.Density / 100 * BiomeAmount);
				for (size_t i = 0; i < MeshesDensity; i++) //spawn for the determined desnity of the mesh spawn that many into the map
				{
					//pick a random location within the specified biome
					int32 RandomLocation = TerrainGenerator->Stream.RandRange(0, BiomePoints.Value.Num() - 1);
					if (BiomePoints.Value.Num() > 0) {
						int32 VertexIndex = BiomePoints.Value[RandomLocation];
						FVector VertexLocation = TerrainGenerator->Vertices[VertexIndex];

						FRotator Rotation = FRotator(0, 0, 0); //give the mesh a random Yaw rotation
						Rotation.Yaw = TerrainGenerator->Stream.FRandRange(0.0f, 360.0f);

						//spawn in a new Actor in specified location, with random rotation
						AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), VertexLocation, Rotation);
						SpawnedMesh->SetMobility(EComponentMobility::Stationary);

						SpawnedMesh->SetActorScale3D(FVector(TerrainGenerator->Stream.FRandRange(15.0f, 45.0f))); //give the mesh a random scale
						SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(DifferentMeshes.Mesh); //assign the appropriate mesh to the spawned in actor

						SpawnedMesh->SetActorEnableCollision(DifferentMeshes.bHasCollision); //update the meshes collision so if say grass it will not have any player collision

						//remove the choosen location from the list so no new meshes can spawn there
						BiomePoints.Value.RemoveAt(RandomLocation);
						MeshActors.Add(SpawnedMesh); //add the mesh to the list of all meshes within the map
					}
				}
			}
		}
	}
}
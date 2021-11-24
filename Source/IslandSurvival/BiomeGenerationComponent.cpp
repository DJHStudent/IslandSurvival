// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerationComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Async/AsyncWork.h"
#include "ZombieSpawner.h"

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
		TerrainGenerator->VerticeColours[CurrentVertexPosition] = BiomeStatsMap[1].GetDefaultObject()->BiomeColour; //as underwater set biome to ocean
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
			if (!HasHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, RandomBiome, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
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
	TArray<TPair<int32, FVector2D>> BiomePositions = PoissonDiskSampling::CreatePoints(SingleIslandMaxSize, 30, IslandWidth, IslandHeight, IslandVertexIdentifiers.Value.MinXPosition * TerrainGenerator->GridSize, IslandVertexIdentifiers.Value.MinYPosition * TerrainGenerator->GridSize, BiomeStatsMap, TerrainGenerator->Stream);


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
		if (!HasHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, NearestBiome, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
			UpdateBiomeLists(NearestBiome, VertexIdentifier); //use the non-height based biome to update
	}
}

bool UBiomeGenerationComponent::HasHeightBiomes(float ZHeight, int32 Biome, int32 VertexIdentifier)
{
	if (ZHeight > 900) //check if the Z position of the point is above the specified value
	{
		//two heigh biomes have keys of 5 and 6 respectivly
		for (int32 NeighbourBiome : BiomeStatsMap[5].GetDefaultObject()->NeighbourBiomeKeys) //check the possible neighbour biomes for 5 first
		{
			if (NeighbourBiome == Biome) //if the lower elevation biome is a neighbour then use 5 to update the list
			{
				UpdateBiomeLists(5, VertexIdentifier);
				return true; //as biome found return true
			}
		}

		//just do same again but as not biome 5 test it with biome 6
		for (int32 NeighbourBiome : BiomeStatsMap[6].GetDefaultObject()->NeighbourBiomeKeys)
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

	TerrainGenerator->VerticeColours[VertexIdentifier] = BiomeStatsMap[Biome].GetDefaultObject()->BiomeColour; //for the specified biome assign the vertex the appropriate colour
	BiomeAtEachPoint[VertexIdentifier] = Biome; //also give each vertex the appropriate biome

	//get the grid location of the current vertex
	int32 XPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].X / TerrainGenerator->GridSize);
	int32 YPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].Y / TerrainGenerator->GridSize);

	//assign the appropriate height value to the vertex
	if (BiomeStatsMap[Biome].GetDefaultObject()->bCustomTerrain) //if the biome has custom terrain
	{
		float NewZPos = BiomeStatsMap[Biome].GetDefaultObject()->TerrainHeight->GenerateHeight(XPos, YPos, TerrainGenerator->bSmoothTerrain); //calculate a new Z height for it
		TerrainGenerator->Vertices[VertexIdentifier].Z = NewZPos; //assign it as the new vertex height
	}


	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list holding each biome and the vertices contained within them, if they exist
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list as it has not been seen before
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}

	BiomeLerping(YPos, XPos); //also blend with a neighbour biome to get good transitions
}

void UBiomeGenerationComponent::BiomeLerping(int32 i, int32 j) //blend 2 neighbouring biome points together so smoother transition
{
	int32 VertexIndex = i * TerrainGenerator->Width + j; //this is the current point at which is being checked
	float VertexValue = TerrainGenerator->Vertices[VertexIndex].Z;
	int32 blendAmount = 1; //how far away a neighbour can actually be

	//chech all neighbours of current vertex
	for (int32 i1 = -blendAmount; i1 <= blendAmount; i1++) //loop through all neighbouring grid points
	{
		for (int32 j1 = -blendAmount; j1 <= blendAmount; j1++)
		{
			//as long as theneighbour falls within the bounds of the island it can be lerped
			if (j + j1 >= 0 && j + j1 < TerrainGenerator->Width && i + i1 >= 0 && i + i1 < TerrainGenerator->Height)
			{
				//check the biome around the point
				int32 NeighbourIndex = (i + i1) * TerrainGenerator->Width + (j + j1); //this is the index of the value of the neighbouring currently checking
				float NeighbourValue = TerrainGenerator->Vertices[NeighbourIndex].Z;
				int32 CurrBiome = BiomeAtEachPoint[VertexIndex];
				if (BiomeAtEachPoint[VertexIndex] != BiomeAtEachPoint[NeighbourIndex] //if the two biomes are different, and are one of the onces which can actually lerp
					&& BiomeAtEachPoint[VertexIndex] != 5 && BiomeAtEachPoint[NeighbourIndex] != 5
					&& BiomeAtEachPoint[VertexIndex] != 6 && BiomeAtEachPoint[NeighbourIndex] != 6
					&& BiomeAtEachPoint[VertexIndex] != 2 && BiomeAtEachPoint[NeighbourIndex] != 2
					&& BiomeAtEachPoint[VertexIndex] != 3 && BiomeAtEachPoint[NeighbourIndex] != 3
					&& BiomeAtEachPoint[VertexIndex] != -1 && BiomeAtEachPoint[NeighbourIndex] != -1)
				{
					if (!bBeenLerped[VertexIndex]) //if this vertex has not yet been lerped
					{
						float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[NeighbourIndex].Z, VertexValue, 0.5f); //for vertex directly next to the new get value helpway between the 2
						if (BiomeAtEachPoint[VertexIndex] == 1 || BiomeAtEachPoint[NeighbourIndex] == 1) //as next to ocean, update the biome to be a beach
						{
							BiomeAtEachPoint[VertexIndex] = 13; //set the biome as a beach
							TerrainGenerator->VerticeColours[VertexIndex] = BiomeStatsMap[13].GetDefaultObject()->BiomeColour;
							TerrainGenerator->Vertices[VertexIndex].Z = WaterLine; //move Z height to be at the water
						}
						else //as not near ocean
						{
							TerrainGenerator->Vertices[VertexIndex].Z = LerpedValue; //set value of vertex to be the lerped one
							//get the biome colour of both points
							FLinearColor VertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[VertexIndex]].GetDefaultObject()->BiomeColour;
							FLinearColor NeighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].GetDefaultObject()->BiomeColour;
							TerrainGenerator->VerticeColours[VertexIndex] = FMath::Lerp(VertexBiomeColor, NeighbourBiomeColor, 0.25f); //blend the colours, using the Vertex as the main colour
						}
						bBeenLerped[VertexIndex] = true; //update the value so can no longer be blended again
					}
					if (!bBeenLerped[NeighbourIndex]) //repeate the above, but for the neighbours vertex, if it hasn't yet been lerped
					{
						float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[NeighbourIndex].Z, VertexValue, 0.5f); //for vertex directly next to the new biome
						if (BiomeAtEachPoint[NeighbourIndex] == 1 || BiomeAtEachPoint[VertexIndex] == 1)
						{
							BiomeAtEachPoint[NeighbourIndex] = 13; //set the biome as a beach
							TerrainGenerator->VerticeColours[NeighbourIndex] = BiomeStatsMap[13].GetDefaultObject()->BiomeColour;
							TerrainGenerator->Vertices[NeighbourIndex].Z = WaterLine;
						}
						else
						{
							TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;
							FLinearColor VertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[VertexIndex]].GetDefaultObject()->BiomeColour;
							FLinearColor NeighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].GetDefaultObject()->BiomeColour;
							TerrainGenerator->VerticeColours[NeighbourIndex] = FMath::Lerp(VertexBiomeColor, NeighbourBiomeColor, 0.75f);
						}
						bBeenLerped[NeighbourIndex] = true;
					}
				}
			}
		}
	}
}

void UBiomeGenerationComponent::SpawnStructure()
{
	int32 StructureAmount = FMath::FloorToInt(TerrainGenerator->Width * TerrainGenerator->Height / 1000); //determine number to spawn in based on map size, rounding if ends up being a float
	//* divide by 1000 so that if say have a 100 by 100 map will have 10 tents spawn on it
	
	TArray<FVector2D> GridPoints; //list of all points where structures can spawn

	//how far appart on the X and Y plane do these GridPoints appear ensuring that both will directions contain exactly Structure amount of points
	float XDistAppart = TerrainGenerator->Width / StructureAmount;
	float YDistAppart = TerrainGenerator->Height / StructureAmount;
	for (int32 y = 0; y < StructureAmount; y++) //initilize the 2d grid so points can be made where the structures will be spawning in 
	{
		for (int32 x = 0; x < StructureAmount; x++)
		{
			GridPoints.Add(FVector2D(x * XDistAppart, y * YDistAppart));
		}
	}

	for (int32 i = 0; i < StructureAmount; i++) //for the number of structure which can spawn in
	{
		int32 RandomIndex = TerrainGenerator->Stream.RandRange(0, GridPoints.Num() - 1); //get random index of the grid

		int32 XPosition = FMath::Clamp(FMath::CeilToInt(GridPoints[RandomIndex].X), 0, TerrainGenerator->Width - 1);
		int32 YPosition = FMath::Clamp(FMath::CeilToInt(GridPoints[RandomIndex].Y), 0, TerrainGenerator->Height - 1);

		int32 VertexIndex = YPosition * TerrainGenerator->Width + XPosition; //get index in vertices array of the point
		FVector VertexLocation = TerrainGenerator->Vertices[VertexIndex]; //get its actual location

		if (VertexLocation.Z < WaterLine) //if spawning in the ocean
			VertexLocation.Z = WaterLine - 90; //update the spawn location to be just below water surface, not on the surface
		else
		{
			//ensure tent is on flat ground and not in a wall
			for (int32 a = -1; a <= 1; a++) //loop through the 8 neighbouring vertices of current one
			{
				for (int32 b = -1; b <= 1; b++) 
				{
					if (XPosition + b >= 0 && XPosition + b < TerrainGenerator->Width && YPosition + a >= 0 && YPosition + a < TerrainGenerator->Height)
					{ //if neighbour actually on map
						int32 NeighbourIndex = (a + YPosition) * TerrainGenerator->Width + (b + XPosition);
						TerrainGenerator->Vertices[NeighbourIndex].Z = TerrainGenerator->Vertices[VertexIndex].Z; //adjust its position so ground all have same Z Position as the central vertex
					}
				}
			}
		}

		//spawn in empty mesh at given location
		AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), VertexLocation, FRotator::ZeroRotator);
		SpawnedMesh->SetNetDormancy(ENetDormancy::DORM_DormantAll); //do not continue to replicate it to clients
		SpawnedMesh->SetMobility(EComponentMobility::Stationary); //ensure it cannot move

		if (GetWorld()->IsServer()) //only spawn spawners in on the server version
			SpawnZombieSpawner(VertexLocation, VertexIndex);

		if (TerrainGenerator->Vertices[VertexIndex].Z < 0) //spawn in a bouy as must be underwater
		{
			SpawnedMesh->SetActorScale3D(FVector(10)); //give the bouy a certain scale
			SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(Bouy); //assign the appropriate mesh to the spawned in actor
		}
		else //as on land spawn in a tent
		{
			SpawnedMesh->SetActorScale3D(FVector(40)); //give the bouy a certain scale
			SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(Tent); //assign the appropriate mesh to the spawned in actor
		}

		//add the meshes to the list of all meshes within the map so will be destroyed when resetting map
		MeshActors.Add(SpawnedMesh);

		GridPoints.RemoveAt(RandomIndex);
	}
}


void UBiomeGenerationComponent::SpawnMeshes() //spawn in the plants into the map
{
	for (auto& BiomePoints : VertexBiomeLocationsMap) //for each biome on the map
	{
		if (BiomeStatsMap[BiomePoints.Key].GetDefaultObject()->BiomeMeshes.Num() > 0) //only do as long as biome contains meshes to be spawned in
		{
			int32 BiomeAmount = BiomePoints.Value.Num(); //number of points which make up the biome
			for (FBiomeMeshes DifferentMeshes : BiomeStatsMap[BiomePoints.Key].GetDefaultObject()->BiomeMeshes) //for each mesh which can spawn in at the current biome
			{
				//calculate number of each mesh to spawn in based on its % density of total points
				int32 MeshesDensity = FMath::CeilToInt(DifferentMeshes.Density / 100 * BiomeAmount);
				int32 MeshesAdded = 0;
				while (MeshesAdded < MeshesDensity) //spawn for the determined desnity of the mesh spawn that many into the map
				{
					//pick a random location within the specified biome
					int32 RandomLocation = TerrainGenerator->Stream.RandRange(0, BiomePoints.Value.Num() - 1);

					//get all points within a certain radius of the choosen point
					TArray<int32> RadiusPoints; //holds the index of the point within the vertices array
					if (BiomePoints.Value.Num() > 0)
					{
						int32 VertexIndex = BiomePoints.Value[RandomLocation];
						int32 XCentre = FMath::Clamp(FMath::CeilToInt(TerrainGenerator->Vertices[VertexIndex].X / TerrainGenerator->GridSize), 0, TerrainGenerator->Width - 1);
						int32 YCentre = FMath::Clamp(FMath::CeilToInt(TerrainGenerator->Vertices[VertexIndex].Y / TerrainGenerator->GridSize), 0, TerrainGenerator->Height - 1);
						if (DifferentMeshes.NeighbourRadius == 0)
							RadiusPoints.Add(VertexIndex);
						else
						{
							for (int32 a = -DifferentMeshes.NeighbourRadius; a < DifferentMeshes.NeighbourRadius; a++) //add all points within a 2 radius of the choosen one
							{
								for (int32 b = -DifferentMeshes.NeighbourRadius; b < DifferentMeshes.NeighbourRadius; b++)
								{
									if (XCentre + b >= 0 && XCentre + b < TerrainGenerator->Width && YCentre + a >= 0 && YCentre + a < TerrainGenerator->Height)
									{ //as long as the point is actually on the map
										int32 NeighbourIndex = (a + YCentre) * TerrainGenerator->Width + (b + XCentre);
										if (BiomePoints.Key == BiomeAtEachPoint[NeighbourIndex]) //ensure neighbour point will be the same biome
										{
											//now can actually add the point to the radius, as long as exists
											if (BiomePoints.Value.Contains(NeighbourIndex))
												RadiusPoints.Add(NeighbourIndex);
										}
									}
								}
							}
						}

						//get a random amount of the points to use, 1 so will always use the centre point
						int32 MaxRadiusLocations = TerrainGenerator->Stream.RandRange(DifferentMeshes.MinNeighbours, DifferentMeshes.MaxNeighbours);
						for (int32 k = 0; k < MaxRadiusLocations + 1; k++)
						{
							RandomLocation = TerrainGenerator->Stream.RandRange(0, RadiusPoints.Num() - 1);
							if (BiomePoints.Value.Num() > 0 && RadiusPoints.Num() > 0 && MeshesAdded <= MeshesDensity) 
							{
								VertexIndex = RadiusPoints[RandomLocation];
								FVector VertexLocation = MeshLocation(TerrainGenerator->Vertices[VertexIndex]); //adjust the location so somewhat offset

								FRotator Rotation = FRotator(0, 0, 0); //give the mesh a random Yaw rotation
								Rotation.Yaw = TerrainGenerator->Stream.FRandRange(0.0f, 360.0f);

								//spawn in a new Actor in specified location, with random rotation
								AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), VertexLocation, Rotation);
								SpawnedMesh->NetDormancy = ENetDormancy::DORM_DormantAll;
								SpawnedMesh->SetMobility(EComponentMobility::Stationary);

								SpawnedMesh->SetActorScale3D(FVector(TerrainGenerator->Stream.FRandRange(15.0f, 45.0f))); //give the mesh a random scale
								SpawnedMesh->GetStaticMeshComponent()->SetStaticMesh(DifferentMeshes.Mesh); //assign the appropriate mesh to the spawned in actor

								SpawnedMesh->SetActorEnableCollision(DifferentMeshes.bHasCollision); //update the meshes collision so if say grass it will not have any player collision

								//remove the choosen location from the list so no new meshes can spawn there
								BiomePoints.Value.Remove(VertexIndex);
								RadiusPoints.RemoveAt(RandomLocation);
								MeshActors.Add(SpawnedMesh); //add the mesh to the list of all meshes within the map

								MeshesAdded++;
							}
						}
					}
				}
			}
		}
	}
}

FVector UBiomeGenerationComponent::MeshLocation(FVector VertexPosition) //in a square around the vertex spawning at, randomly place the mesh, so not appearing as a grid like pattern
{

	//get the index location of the point, using its actual location
	int32 XIndex = FMath::FloorToInt(VertexPosition.X / TerrainGenerator->GridSize);
	int32 YIndex = FMath::FloorToInt(VertexPosition.Y / TerrainGenerator->GridSize);

	//if a random x offset to be at max halfway between this point and its 2 neighbouring ons(in X direction)
	float RandXPosition = TerrainGenerator->Stream.RandRange(VertexPosition.X - TerrainGenerator->GridSize / 2, VertexPosition.X + TerrainGenerator->GridSize / 2);
	
	//as this offset could result in mesh floating in the air, perform a lerp so blend it with its appropriate neighbour's vertex
	if (RandXPosition < VertexPosition.X) //as X choosen is going backwards, do for neighbour one back
	{
		if (XIndex - 1 >= 0) //ensure point actually on map
		{
			float NeighbourZ = TerrainGenerator->Vertices[YIndex * TerrainGenerator->Width + (XIndex - 1)].Z;
			//just lerp Z position between vertex pos and neighbour pos based on how far away it is
			float Alpha = FMath::Abs(VertexPosition.X - RandXPosition) / TerrainGenerator->GridSize; //how much of each value to include when lerping
			float NewZPosition = FMath::Lerp(VertexPosition.Z, NeighbourZ, Alpha); //lerp between values, so that meshes Z position will be on the ground
			VertexPosition.Z = NewZPosition;
		}
	}
	else if (RandXPosition > VertexPosition.X) //as X choosen is going forward, do for neighbour one back
	{
		if (XIndex + 1 < TerrainGenerator->Width) //ensure point actually on map
		{
			float NeighbourZ = TerrainGenerator->Vertices[YIndex * TerrainGenerator->Width + (XIndex + 1)].Z; //get Z height of neighbour
			//just lerp Z position between vertex pos and neighbour pos based on how far away it is

			float Alpha = FMath::Abs(VertexPosition.X - RandXPosition) / TerrainGenerator->GridSize; //how much of each value to include when lerping
			float NewZPosition = FMath::Lerp(VertexPosition.Z, NeighbourZ, Alpha); //lerp between values, so that meshes Z position will be on the ground
			VertexPosition.Z = NewZPosition;
		}
	}


	//if a random y offset to be at max halfway between this point and its 2 neighbouring ons(in y direction)
	float RandYPosition = TerrainGenerator->Stream.RandRange(VertexPosition.Y - TerrainGenerator->GridSize / 2, VertexPosition.Y + TerrainGenerator->GridSize / 2);
	if (RandYPosition < VertexPosition.Y) //as Y choosen is going backwards, do for neighbour one back
	{
		if (YIndex - 1 >= 0) //ensure point actually on map
		{
			float NeighbourZ = TerrainGenerator->Vertices[(YIndex - 1) * TerrainGenerator->Width + XIndex].Z;
			//just lerp Z position between vertex pos and neighbour pos based on how far away it is
			float Alpha = FMath::Abs(VertexPosition.Y - RandYPosition) / TerrainGenerator->GridSize; //how much of each value to include when lerping
			float NewZPosition = FMath::Lerp(VertexPosition.Z, NeighbourZ, Alpha); //lerp between values, so that meshes Z position will be on the ground
			VertexPosition.Z = NewZPosition;
		}
	}	
	else if (RandYPosition > VertexPosition.Y) //as Y choosen is going forward, do for neighbour one back
	{
		if (YIndex + 1 < TerrainGenerator->Height)  //ensure point actually on map
		{
			float NeighbourZ = TerrainGenerator->Vertices[(YIndex + 1) * TerrainGenerator->Width + XIndex].Z;

			float Alpha = FMath::Abs(VertexPosition.Y - RandYPosition) / TerrainGenerator->GridSize; //how much of each value to include when lerping
			float NewZPosition = FMath::Lerp(VertexPosition.Z, NeighbourZ, Alpha); //lerp between values, so that meshes Z position will be on the ground
			VertexPosition.Z = NewZPosition;
		}
	}

	//update the new VertexPosition with the appropriate values for X and Y
	VertexPosition.X = RandXPosition;
	VertexPosition.Y = RandYPosition;
	return VertexPosition;
}

void UBiomeGenerationComponent::SpawnZombieSpawner(FVector Location, int32 Index)
{
	float SpawnerXOffset = TerrainGenerator->GridSize; //shift point so will spawn more in front of the structure not no it
	float SpawnerZOFsset = 500; //ensures spawner will spawn enemes above the structure
	FVector SpawnerLocation = FVector(Location.X + SpawnerXOffset, Location.Y, Location.Z + SpawnerZOFsset);

	//spawn in a Zombie Spawner actor into the map, at the specified location
	AZombieSpawner* ZombieSpawner = GetWorld()->SpawnActor<AZombieSpawner>(TerrainGenerator->ZombieSpawner, SpawnerLocation, FRotator::ZeroRotator);
	ZombieSpawner->SetNetDormancy(ENetDormancy::DORM_DormantAll); //do not continue to replicate it to clients

	int32 BiomeKey = BiomeAtEachPoint[Index]; //get the biome of the current point
	ZombieSpawner->ZombieScale = BiomeStatsMap[BiomeKey].GetDefaultObject()->Scale;
	ZombieSpawner->ZombieSwimSpeed = BiomeStatsMap[BiomeKey].GetDefaultObject()->SwimSpeed;
	ZombieSpawner->ZombieWalkSpeed = BiomeStatsMap[BiomeKey].GetDefaultObject()->WalkSpeed;

	MeshActors.Add(ZombieSpawner);
}
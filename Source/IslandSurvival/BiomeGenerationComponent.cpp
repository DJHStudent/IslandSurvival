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
	TArray<TPair<int32, FVector2D>> BiomePositions = DiskSampling.CreatePoints(SingleIslandMaxSize, 30, IslandWidth, IslandHeight, IslandVertexIdentifiers.Value.MinXPosition * TerrainGenerator->GridSize, IslandVertexIdentifiers.Value.MinYPosition * TerrainGenerator->GridSize, BiomeStatsMap, TerrainGenerator->Stream);


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

	//assign the appropriate height value to the vertex
	//desert, alpine, forest, dryland, dead forest
	if (BiomeStatsMap[Biome].GetDefaultObject()->bCustomTerrain)// || Biome == 12)
	{
		//BiomeStatsMap[Biome].BiomeHeight.TerraceSize = TerrainGenerator->TerraceSize;
		int32 XPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].X / TerrainGenerator->GridSize);
		int32 YPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].Y / TerrainGenerator->GridSize);
		float NewZPos = BiomeStatsMap[Biome].GetDefaultObject()->TerrainHeight->GenerateHeight(XPos, YPos);
		//UE_LOG(LogTemp, Error, TEXT("Cusom BIome Mesh: %f"), ZPos)
		TerrainGenerator->Vertices[VertexIdentifier].Z = NewZPos;
		//TerrainGenerator->Vertices[VertexIdentifier].Z = FMath::RoundFromZero(TerrainGenerator->Vertices[VertexIdentifier].Z * TerrainGenerator->TerraceSize) / TerrainGenerator->TerraceSize;//terrace the terrain by rouding each points height to its nearest multiple of TerraceSize
	}


	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list holding each biome and the vertices contained within them, if they exist
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list as it has not been seen before
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}
}

void UBiomeGenerationComponent::BiomeLerping()
{
	TArray<bool> bBeenLerped;
	bBeenLerped.Init(false, TerrainGenerator->Width * TerrainGenerator->Height);
	for (int32 i = 0; i < TerrainGenerator->Height; i++)
	{
		for (int32 j = 0; j < TerrainGenerator->Width; j++)
		{
			int32 VertexIndex = i * TerrainGenerator->Width + j; //this is the current point at which is being checked
			float VertexValue = TerrainGenerator->Vertices[VertexIndex].Z;
			int32 blendAmount = 1;

			//chech all neighbours of current vertex
			for (int32 i1 = -blendAmount; i1 <= blendAmount; i1++) //loop through all neighbouring grid points
			{
				for (int32 j1 = -blendAmount; j1 <= blendAmount; j1++)
				{
					//as long as the new point falls within the bounds of the island it can be used
					if (j + j1 >= 0 && j + j1 < TerrainGenerator->Width && i + i1 >= 0 && i + i1 < TerrainGenerator->Height) //2 so edges do not get blending and will just fall off
					{
						//check the biome around the point
						int32 NeighbourIndex = (i + i1) * TerrainGenerator->Width + (j + j1); //this is the index of the value of the neighbouring biome
						float NeighbourValue = TerrainGenerator->Vertices[NeighbourIndex].Z;
						//float NeightbourValue = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].BiomeHeight.GenerateHeight(i, j);//->GenerateHeight();
						int32 CurrBiome = BiomeAtEachPoint[VertexIndex];
						if (BiomeAtEachPoint[VertexIndex] != BiomeAtEachPoint[NeighbourIndex] 
							//&& BiomeAtEachPoint[VertexIndex] != 1 && BiomeAtEachPoint[NeighbourIndex] != 1
							//&& BiomeAtEachPoint[VertexIndex] != 7 && BiomeAtEachPoint[NeighbourIndex] != 7
							&& BiomeAtEachPoint[VertexIndex] != 5 && BiomeAtEachPoint[NeighbourIndex] != 5
							&& BiomeAtEachPoint[VertexIndex] != 6 && BiomeAtEachPoint[NeighbourIndex] != 6
							&& BiomeAtEachPoint[VertexIndex] != 2 && BiomeAtEachPoint[NeighbourIndex] != 2
							&& BiomeAtEachPoint[VertexIndex] != 3 && BiomeAtEachPoint[NeighbourIndex] != 3)
						{
							if (!bBeenLerped[VertexIndex])// && !bBeenLerped[NeighbourIndex])
							{
								float alpha = 0.5f;
								if (i1 != 0 && j1 != 0)
									alpha = 0.5f / (FMath::Pow(i1, 2) + FMath::Pow(j1, 2));//FMath::Max(FMath::Abs(i1), FMath::Abs(i1));
								//	alpha = 0.25f;
								//float OtherBiomeValueIfNeightbourOtherBiome = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].BiomeHeight.GenerateHeight(j + j1, i + i1);
								float LerpedValue = FMath::Lerp(VertexValue, TerrainGenerator->Vertices[NeighbourIndex].Z, 0.5f); //for vertex directly next to the new biome
								TerrainGenerator->Vertices[VertexIndex].Z = LerpedValue;
								LerpedValue = FMath::Lerp(VertexValue, TerrainGenerator->Vertices[NeighbourIndex].Z, 0.75f); //for vertex directly next to the new biome
								//TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;

								FLinearColor vertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[VertexIndex]].GetDefaultObject()->BiomeColour;
								FLinearColor neighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].GetDefaultObject()->BiomeColour;
								TerrainGenerator->VerticeColours[VertexIndex] = FMath::Lerp(vertexBiomeColor, neighbourBiomeColor, 0.25f);//FLinearColor(0.5f, 0.5f, 0.5f);
								//TerrainGenerator->VerticeColours[NeighbourIndex] = FMath::Lerp(neighbourBiomeColor, vertexBiomeColor, 0.25f);//FLinearColor(0.5f, 0.5f, 0.5f);

								//check all neighbours of the neighbour of the current vertex
								////for (int32 a = -blendAmount; a <= blendAmount; a++) //get neighbouring vertices of current one which are same biome
								////{
								////	for (int32 b = -blendAmount; b <= blendAmount; b++)
								////	{
								////		if (j1 + b >= 0 && j1 + b < TerrainGenerator->Width && i1 + a >= 0 && i1 + a < TerrainGenerator->Height)
								////		{
								////			int32 SameNeighbourIndex = (i1 + a) * TerrainGenerator->Width + (j1 + b);
								////			//lerp between the two values
								////			if (BiomeAtEachPoint[VertexIndex] == BiomeAtEachPoint[SameNeighbourIndex] && !bBeenLerped[SameNeighbourIndex])// && !bBeenLerped[SameNeighbourIndex])
								////			{
								////				//UE_LOG(LogTemp, Error, TEXT("Same Biome"))
								////				//if (i1 == 3 || i1 == -3)
								////				//	alpha = 0.75f;
								////				if (a != 0 && b != 0)
								////					alpha = 0.5f / FMath::Max(FMath::Abs(a), FMath::Abs(b));//FVector2D::DistSquared(FVector2D(a, b), FVector2D(i1, j1));//FMath::Max(FMath::Abs(a), FMath::Abs(b));
								////				float NewLerpedValue = FMath::Lerp(NeighbourValue, TerrainGenerator->Vertices[SameNeighbourIndex].Z, alpha); //for vertex directly next to the new biome
								////				TerrainGenerator->Vertices[SameNeighbourIndex].Z = NewLerpedValue;
								////				bBeenLerped[SameNeighbourIndex] = true;

								////			}
								////		}
								////	}
								////}

								bBeenLerped[VertexIndex] = true;
								//bBeenLerped[NeighbourIndex] = true;
							}
							//if (!bBeenLerped[NeighbourIndex])
							//{
							//	//UE_LOG(LogTemp, Error, TEXT("Biomes: %f, %i, %i, %i"), NeighbourValue, VertexIndex, BiomeAtEachPoint[VertexIndex], BiomeAtEachPoint[NeighbourIndex])
							//	/*	float alpha = 0.5f;
							//		if (i1 != 0 && j1 != 0) {
							//			alpha = 1 - 0.5f / FMath::Max(FMath::Abs(i1), FMath::Abs(i1));
							//		}
							//			alpha = FMath::Clamp(alpha, 0.5f, 1.0f);*/
							//			//	if (i1 != 0 && j1 != 0)
							//				//	alpha =1 - 0.5f / FVector2D::Distance(FVector2D(i, j), FVector2D(i1, j1));//Max(FMath::Abs(i1), FMath::Abs(j1));
							//				//if (i1 == 3 || i1 == -3)
							//				//	alpha = 0.75f;
							//			//float OtherBiomeValueIfNeightbourOtherBiome = BiomeStatsMap[CurrBiome].BiomeHeight.GenerateHeight(j + j1, i + i1);
							//			float LerpedValue = FMath::Lerp(VertexValue, TerrainGenerator->Vertices[NeighbourIndex].Z, 0.5f); //for vertex directly next to the new biome
							//			TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;

							//			//TerrainGenerator->VerticeColours[VertexIndex] = FMath::Lerp(vertexBiomeColor, neighbourBiomeColor, 0.25f);//FLinearColor(0.5f, 0.5f, 0.5f);

							//			////for (int32 a = -blendAmount; a <= blendAmount; a++) //get neighbouring vertices of current one which are same biome
							//			////{
							//			////	for (int32 b = -blendAmount; b <= blendAmount; b++)
							//			////	{
							//			////		if (j1 + b >= 0 && j1 + b < TerrainGenerator->Width && i1 + a >= 0 && i1 + a < TerrainGenerator->Height)
							//			////		{
							//			////			int32 SameNeighbourIndex = (i1 + a) * TerrainGenerator->Width + (j1 + b);
							//			////			//lerp between the two values
							//			////			if (BiomeAtEachPoint[VertexIndex] == BiomeAtEachPoint[SameNeighbourIndex] && !bBeenLerped[SameNeighbourIndex])// && !bBeenLerped[SameNeighbourIndex])
							//			////			{
							//			////				//UE_LOG(LogTemp, Error, TEXT("Same Biome"))
							//			////					//if (i1 == 3 || i1 == -3)
							//			////					//	alpha = 0.75f;
							//			////					//if (i1 != 0 && j1 != 0)
							//			////					//alpha = 0.5f / FVector2D::Distance(FVector2D(a, b), FVector2D(i1, j1));//Max(FMath::Abs(i1), FMath::Abs(j1));
							//			////				if (a != 0 && b != 0)
							//			////					alpha = 1 - 0.5f / FMath::Max(FMath::Abs(a), FMath::Abs(b));//FVector2D::DistSquared(FVector2D(a, b), FVector2D(i1, j1));
							//			////				float NewLerpedValue = FMath::Lerp(NeighbourValue, TerrainGenerator->Vertices[SameNeighbourIndex].Z, alpha); //for vertex directly next to the new biome
							//			////				TerrainGenerator->Vertices[SameNeighbourIndex].Z = NewLerpedValue;
							//			////				bBeenLerped[SameNeighbourIndex] = true;

							//			////			}
							//			////		}
							//			////	}
							//			////}


							//			bBeenLerped[NeighbourIndex] = true;
							//		
							//}
						}
					}
				}
			}
		}

		//uj

		//////////chech all neighbours of current vertex if two away is a different biome
		////////for (int32 i1 = -2; i1 <= 2; i1 ++) //loop through all neighbouring grid points
		////////{
		////////	for (int32 j1 = -2; j1 <= 2; j1 ++)
		////////	{
		////////		//as long as the new point falls within the bounds of the island it can be used
		////////		if (j + j1 >= 0 && j + j1 < TerrainGenerator->Width && i + i1 >= 0 && i + i1 < TerrainGenerator->Height)
		////////		{
		////////			//check the biome around the point
		////////			int32 NeighbourIndex = (i + i1) * TerrainGenerator->Width + (j + j1);
		////////			//int32 NewVertexIndex = (i - i1) * TerrainGenerator->Width + (j - j1);
		////////			if (BiomeAtEachPoint[VertexIndex] != BiomeAtEachPoint[NeighbourIndex])
		////////			{
		////////				if (!bBeenLerped[VertexIndex])
		////////				{

		////////					float alpha = 0.5f;
		////////					if (i1 != 0) {
		////////						alpha /= i1;
		////////						float LerpedValue = FMath::Lerp(VertexValue, TerrainGenerator->Vertices[NeighbourIndex].Z, alpha);
		////////						TerrainGenerator->Vertices[VertexIndex].Z = LerpedValue;
		////////						bBeenLerped[VertexIndex] = true;
		////////					}
		////////				}
		////////				if (!bBeenLerped[NeighbourIndex])
		////////				{
		////////					float alpha = 0.5f;
		////////					if (i1 != 0) {
		////////						alpha = 1 - 0.5f / i1;
		////////						float LerpedValue = FMath::Lerp(VertexValue, TerrainGenerator->Vertices[NeighbourIndex].Z, alpha);
		////////						TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;
		////////						bBeenLerped[NeighbourIndex] = true;
		////////					}
		////////				}

		////////			}
		////////		}
		////////	}
		////////}

		//for (int32 i1 = -3; i1 <= 3; i1 += 3) //loop through all neighbouring grid points
		//{
		//	for (int32 j1 = -3; j1 <= 3; j1 += 3)
		//	{
		//		//as long as the new point falls within the bounds of the island it can be used
		//		if (j + j1 >= 0 && j + j1 < TerrainGenerator->Width && i + i1 >= 0 && i + i1 < TerrainGenerator->Height)
		//		{
		//			//check the biome around the point
		//			int32 NeighbourIndex = (i + i1) * TerrainGenerator->Width + (j + j1);
		//			if (BiomeAtEachPoint[VertexIndex] != BiomeAtEachPoint[NeighbourIndex])
		//			{
		//				if (!bBeenLerped[VertexIndex])
		//				{
		//					float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[VertexIndex].Z, TerrainGenerator->Vertices[NeighbourIndex].Z, 0.125f);
		//					TerrainGenerator->Vertices[VertexIndex].Z = LerpedValue;
		//					bBeenLerped[NeighbourIndex] = true;
		//				}
		//				if (!bBeenLerped[NeighbourIndex])
		//				{
		//					float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[VertexIndex].Z, TerrainGenerator->Vertices[NeighbourIndex].Z, 0.875f);
		//					TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;
		//					bBeenLerped[NeighbourIndex] = true;
		//				}

		//			}
		//		}
		//	}
		//}
	}

}


void UBiomeGenerationComponent::SpawnMeshes() //spawn in the meshes into the map
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
// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeGenerationComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Async/AsyncWork.h"
#include "ZombieSpawner.h"
#include "FuelPellet.h"
#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"

//declare stats
#if WITH_EDITOR
DECLARE_CYCLE_STAT(TEXT("Single Biomes"), STAT_SingleBiomes, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Multi Biomes"), STAT_MultiBiomes, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Height Biomes"), STAT_HeightBiomes, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Spawn Meshes"), STAT_SpawnMeshes, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Spawn Structures"), STAT_SpawnStructures, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Mesh Location"), STAT_MeshLocation, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Loop through meshes"), STAT_LoopMesh, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Loop through meshes1"), STAT_LoopMesh1, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Loop through meshes2"), STAT_LoopMesh2, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Check Point is within Biome"), STAT_BiomePoint, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Actually Add Mesh to World"), STAT_AddToWorld, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Biome Blending"), STAT_BiomeBlending, STATGROUP_ProcedurallyGeneratedTerrain);
#endif


// Sets default values for this component's properties
UBiomeGenerationComponent::UBiomeGenerationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	FeatureKeys = 1;
	SingleFeatureMaxSize = 6000;
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

void UBiomeGenerationComponent::AddBiomePoints(const int32 XPosition, const int32 YPosition, const float ZPosition) //code to determine where each island/lake is in the world
{
	if (ZPosition < WaterLine) //must be a water / lake point
		AddSinglePoint(XPosition, YPosition, EVertexSpawnLocation::Water);
	else //the point is part of the island biomes
		AddSinglePoint(XPosition, YPosition, EVertexSpawnLocation::Land);
}
//-1 is in a lake, 1 is on land
void UBiomeGenerationComponent::AddSinglePoint(const int32 XPosition, const int32 YPosition, const int32 SpawnLocation) //code to determine where each island is in the world
{
	int32 CurrentVertexPosition = YPosition * TerrainGenerator->Width + XPosition; //the position of the vertex within the array

	int32 Point = 0; //the current island key the point is related to
	if (FeaturePointsMap.Num() > 0) //only actually check the neighbour points if they actually exist
	{
		//get vertex one back
		if (XPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->FeatureNumber[YPosition * TerrainGenerator->Width + (XPosition - 1)]; //get a vertex one behind, if exists and determine its island number
			if (NewPoint < 0 && SpawnLocation == EVertexSpawnLocation::Water || NewPoint > 0 && SpawnLocation == EVertexSpawnLocation::Land)//as long as the new point is not invalid, it must then be connected to this island
				Point = NewPoint; //as first direction checking unknown if any other islands are yet nearby
		}

		//get vertex one up and back is j - i, i - 1
		if (XPosition - 1 >= 0 && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->FeatureNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition - 1)]; //get a vertex diagonaly one behind and up, if exists and determine its island number
			if (NewPoint < 0 && SpawnLocation == EVertexSpawnLocation::Water || NewPoint > 0 && SpawnLocation == EVertexSpawnLocation::Land)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (Point != 0 && Point != NewPoint)
					JoinPoints(Point, NewPoint); //for both elements of the IslandPointsMap join them together
				else //must be appart of the same island / lake
					Point = NewPoint;
			}
		}

		//get vertex one up is i - 1
		if (YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->FeatureNumber[(YPosition - 1) * TerrainGenerator->Width + XPosition]; //get a vertex one up, if exists and determine its island number
			if (NewPoint < 0 && SpawnLocation == EVertexSpawnLocation::Water || NewPoint > 0 && SpawnLocation == EVertexSpawnLocation::Land)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (Point != 0 && Point != NewPoint)
					JoinPoints(Point, NewPoint);
				else //must be appart of the same island
					Point = NewPoint;
			}
		}

		//get vertex one up and forward is j + i, i - 1
		if (XPosition + 1 < TerrainGenerator->Width && YPosition - 1 >= 0)
		{
			int32 NewPoint = TerrainGenerator->FeatureNumber[(YPosition - 1) * TerrainGenerator->Width + (XPosition + 1)]; //get a vertex one diagonally up and forward, if exists and determine its island number
			//new point is currently the key relating to the island looking for
			if (NewPoint < 0 && SpawnLocation == EVertexSpawnLocation::Water || NewPoint > 0 && SpawnLocation == EVertexSpawnLocation::Land)
			{
				//if it is next to a point which is land, but a different island then join the two seperate islands as one whole island
				if (Point != 0 && Point != NewPoint)
					JoinPoints(Point, NewPoint);
				else //must be appart of the same island
					Point = NewPoint;

			}
		}
	}
	//once all 4 currently existing neighboruing points are checked add the point into the islands map
	if (Point == 0) //as all points around it are invalid it must be an entirly new feature
	{
		FIslandStats IslandStats;
		int32 KeyLocation = FeatureKeys * SpawnLocation; //done as if < 0 it is a lake feature

		FeaturePointsMap.Add(KeyLocation, IslandStats); //add the point as a new element
		FeaturePointsMap[KeyLocation].VertexIndices.Add(CurrentVertexPosition); //add the vertex's position within the vertices array

		//PointsMap the min and max values for the island with the inital starting values
		FeaturePointsMap[KeyLocation].MinXPosition = XPosition;
		FeaturePointsMap[KeyLocation].MaxXPosition = XPosition;
		FeaturePointsMap[KeyLocation].MinYPosition = YPosition;
		FeaturePointsMap[KeyLocation].MaxYPosition = YPosition;
		TerrainGenerator->FeatureNumber.Add(KeyLocation); //for the new vertex adding add the islands point to it

		FeatureKeys++;//as a new Island has been made will need another new Key for the next island
	}
	else //it is part of an existing feature so add the point to the existing island
	{
		FeaturePointsMap[Point].VertexIndices.Add(CurrentVertexPosition);
		FeaturePointsMap[Point].UpdateIslandBounds(FVector2D(XPosition, YPosition)); //check to see if the islands bounds need to be updated
		TerrainGenerator->FeatureNumber.Add(Point);
	}
}

void UBiomeGenerationComponent::JoinPoints(const int32 Point, const int32 NewPoint) //for 2 islands which are actually one in the list, add them together
{
	for (int32 i = 0; i < FeaturePointsMap[NewPoint].VertexIndices.Num(); i++) //for each vertex index stored in the new point key move to the Island point key
	{
		FeaturePointsMap[Point].VertexIndices.Add(FeaturePointsMap[NewPoint].VertexIndices[i]);
		TerrainGenerator->FeatureNumber[FeaturePointsMap[NewPoint].VertexIndices[i]] = Point;
	}

	//update the min and max positions of the island to reflect the new values if nessesary to do so
	if (FeaturePointsMap[NewPoint].MinXPosition < FeaturePointsMap[Point].MinXPosition)
		FeaturePointsMap[Point].MinXPosition = FeaturePointsMap[NewPoint].MinXPosition;
	if (FeaturePointsMap[NewPoint].MinYPosition < FeaturePointsMap[Point].MinYPosition)
		FeaturePointsMap[Point].MinYPosition = FeaturePointsMap[NewPoint].MinYPosition;

	if (FeaturePointsMap[NewPoint].MaxXPosition > FeaturePointsMap[Point].MaxXPosition)
		FeaturePointsMap[Point].MaxXPosition = FeaturePointsMap[NewPoint].MaxXPosition;
	if (FeaturePointsMap[NewPoint].MaxYPosition > FeaturePointsMap[Point].MaxYPosition)
		FeaturePointsMap[Point].MaxYPosition = FeaturePointsMap[NewPoint].MaxYPosition;

	FeaturePointsMap.Remove(NewPoint); //remove the new feature which is copied to the actual island list
}

void UBiomeGenerationComponent::DeterminePointBiomes() //for all biomes add them to the appropriate list based on spawn location
{
	for (auto& Biome : BiomeStatsMap)
	{
		EBiomeStats::Type SpawnLocation = BiomeStatsMap[Biome.Key].GetDefaultObject()->BiomeSpawningEnum;
		switch (SpawnLocation)
		{
			case EBiomeStats::LandBased:
				if (BiomeStatsMap[Biome.Key].GetDefaultObject()->bOnlySingle)
					SingleLandBiomeKeys.Add(Biome.Key);
				else
					MultiLandBiomeKeys.Add(Biome.Key);
				break;
			case EBiomeStats::WaterBased:
				if (BiomeStatsMap[Biome.Key].GetDefaultObject()->bOnlySingle)
					SingleLakeBiomeKeys.Add(Biome.Key);
				else
					MultiLakeBiomeKeys.Add(Biome.Key);
				break;
			case EBiomeStats::HeightBased:
				HeightBiomeKeys.Add(Biome.Key);
				break;
			default:
				break;
		}
	}
}

void UBiomeGenerationComponent::EachPointsMap()
{
	DeterminePointBiomes();
	for (auto& PointsPair : FeaturePointsMap) //loop through each feature which has been previously found
	{
		//determine the width and height of the points
		float PointsWidth = (PointsPair.Value.MaxXPosition - PointsPair.Value.MinXPosition);
		float PointsHeight = (PointsPair.Value.MaxYPosition - PointsPair.Value.MinYPosition);

		int32 PointsSize = FMath::CeilToInt(PointsWidth * PointsHeight);//rectangular size of the island

		TArray<int32> SingleBiomeKeys = PointsPair.Key < 0 ? SingleLakeBiomeKeys : SingleLandBiomeKeys;
		TArray<int32> MultiBiomeKeys = PointsPair.Key < 0 ? MultiLakeBiomeKeys : MultiLandBiomeKeys;
		if (PointsSize < SingleFeatureMaxSize) //if the island is small it will only have a single biome on it
			SingleBiomePoints(PointsPair, PointsSize, SingleBiomeKeys, MultiBiomeKeys);
		else
			MultiBiomePoints(PointsPair, PointsSize, MultiBiomeKeys);
	}
}

void UBiomeGenerationComponent::SingleBiomePoints(TPair<int32, FIslandStats> PointsVertexIdentifiers, int32 FeatureSize, TArray<int32>& SingleBiomeKeys, TArray<int32>& MultiBiomeKeys)
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_SingleBiomes);
#endif
	int32 RandomBiome = TerrainGenerator->Stream.RandRange(0, MultiBiomeKeys.Num() - 1); //from biome list pick a random one which is also an above water, land(not mountain) biome
	RandomBiome = MultiBiomeKeys[RandomBiome]; //need to ensure do not actually include the specific single biomes only

	for (int32 VertexIdentifier : PointsVertexIdentifiers.Value.VertexIndices) //for each vertex stored in the specific island
	{
		if (!HasHeightBiomes(TerrainGenerator->Vertices[VertexIdentifier].Z, RandomBiome, VertexIdentifier)) //check and update with height biome, if it exists otherwise a non-height biome
		{ //As no height biome found, use a land/water based one instead
			float OnlySingleMaxArea = -1;
			int32 OnlySingleCurrKey = -1;
			for (int32& Key : SingleBiomeKeys) //while picked random biome from the list also check the biomes size
			{
				if (FeatureSize >= BiomeStatsMap[Key].GetDefaultObject()->MinSpawnArea && FeatureSize <= BiomeStatsMap[Key].GetDefaultObject()->MaxSpawnArea
					&& (OnlySingleMaxArea == -1 || BiomeStatsMap[Key].GetDefaultObject()->MaxSpawnArea > OnlySingleMaxArea))
				{
					OnlySingleMaxArea = BiomeStatsMap[Key].GetDefaultObject()->MaxSpawnArea;
					OnlySingleCurrKey = Key;
				}
			}
			if (OnlySingleCurrKey != -1) //if actually found a single biome island
				UpdateBiomeLists(OnlySingleCurrKey, VertexIdentifier);
			else //use the randomly choosen biome as the island's biome
				UpdateBiomeLists(RandomBiome, VertexIdentifier);
		}
	}
}

void UBiomeGenerationComponent::MultiBiomePoints(TPair<int32, FIslandStats> PointsVertexIdentifiers, int32 IslandSize, TArray<int32>& MultiBiomeKeys)
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_MultiBiomes);
#endif
	//for these islands using voronoi noise with an even distribution of the points by poisson disk sampling to determine the locations of the biomes
	//this will create islands with biomes which have an even distribution

	float IslandWidths = (PointsVertexIdentifiers.Value.MaxXPosition - PointsVertexIdentifiers.Value.MinXPosition);
	float IslandHeights = (PointsVertexIdentifiers.Value.MaxYPosition - PointsVertexIdentifiers.Value.MinYPosition);

	//determine the actual size of the rectangular grid covering the island by using its min and max position * by grid size so its their actual real size
	float IslandWidth = (PointsVertexIdentifiers.Value.MaxXPosition - PointsVertexIdentifiers.Value.MinXPosition) * TerrainGenerator->GridSize;
	float IslandHeight = (PointsVertexIdentifiers.Value.MaxYPosition - PointsVertexIdentifiers.Value.MinYPosition) * TerrainGenerator->GridSize;
	
	//use poisson disk sampling here to give a more even distribution of the biomes
	TArray<TPair<int32, FVector2D>> BiomePositions = PoissonDiskSampling::CreatePoints(SingleFeatureMaxSize, 30, IslandWidth, IslandHeight, 
	PointsVertexIdentifiers.Value.MinXPosition * TerrainGenerator->GridSize, PointsVertexIdentifiers.Value.MinYPosition * TerrainGenerator->GridSize, 
		BiomeStatsMap, TerrainGenerator->Stream, MultiBiomeKeys);

	//using a voronoi noise method which for each vertice just determine the biome point it is nearest
	for (int32 VertexIdentifier : PointsVertexIdentifiers.Value.VertexIndices) //for each point stored in the specific island
	{
		int32 NearestBiome = 1; //biome the vertex will be
		float MinDist = TNumericLimits<float>::Max(); //the distance to the current closest biome point
		FVector2D currentLocation = FVector2D(TerrainGenerator->Vertices[VertexIdentifier].X, TerrainGenerator->Vertices[VertexIdentifier].Y); //location of the current vertex in world

		for (int k = 0; k < BiomePositions.Num(); k++) //for each biome which will exist on the island determine the nearest one
		{
			float CurrentDistance = FVector2D::DistSquared(currentLocation, BiomePositions[k].Value); //the distance to the current point checking
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
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_HeightBiomes);
#endif
	for (auto HeightBiome : HeightBiomeKeys) //check all height based biomes to see if any fit the criteria
	{
		//check to see if the biome is a height based biome or not
		if (ZHeight > BiomeStatsMap[HeightBiome].GetDefaultObject()->MinSpawnHeight //check if the vertexes z height is between these 2 values
			&& ZHeight < BiomeStatsMap[HeightBiome].GetDefaultObject()->MaxSpawnHeight)
		{
			//check if this height biome also has at least one valid neighbour
			if (BiomeStatsMap[HeightBiome].GetDefaultObject()->NeighbourBiomeKeys.Num() <= 0) //no neighbours set means any biome is valid neighbour
			{
				UpdateBiomeLists(HeightBiome, VertexIdentifier);
				return true; //as biome found return true
			}
			for (int32 NeighbourBiome : BiomeStatsMap[HeightBiome].GetDefaultObject()->NeighbourBiomeKeys) //check the possible neighbour biomes for the biome
			{
				if (NeighbourBiome == Biome) //if the land / ocean biome is a neighbour
				{
					UpdateBiomeLists(HeightBiome, VertexIdentifier);
					return true; //as biome found return true
				}
			}
		}
	}
	return false; //current height not high enough for it to be a height biome
}

void UBiomeGenerationComponent::UpdateBiomeLists(int32 Biome, int32 VertexIdentifier) //for the vertex assign it the appropriate biome and correct generation
{	
	TerrainGenerator->VerticeColours[VertexIdentifier] = BiomeStatsMap[Biome].GetDefaultObject()->BiomeColour; //for the specified biome assign the vertex the appropriate colour
	BiomeAtEachPoint[VertexIdentifier].Key = Biome; //also give each vertex the appropriate biome
	//get the grid location of the current vertex
	int32 XPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].X / TerrainGenerator->GridSize);
	int32 YPos = FMath::RoundToInt(TerrainGenerator->Vertices[VertexIdentifier].Y / TerrainGenerator->GridSize);

	//assign the appropriate height value to the vertex
	if (BiomeStatsMap[Biome].GetDefaultObject()->bCustomTerrain) //if the biome has custom terrain
	{
		float NewZPos = BiomeStatsMap[Biome].GetDefaultObject()->TerrainHeight->GenerateHeight(XPos, YPos, WaterLine, TerrainGenerator->bSmoothTerrain); //calculate a new Z height for it
		TerrainGenerator->Vertices[VertexIdentifier].Z = NewZPos; //assign it as the new vertex height
	}


	if (VertexBiomeLocationsMap.Contains(Biome)) //update the list holding each biome and the vertices contained within them, if they exist
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	else //add a new biome too the list as it has not been seen before
	{
		VertexBiomeLocationsMap.Add(Biome, TArray<int32>());
		VertexBiomeLocationsMap[Biome].Add(VertexIdentifier);
	}
	BiomeAtEachPoint[VertexIdentifier].Value = VertexBiomeLocationsMap[Biome].Num() - 1; //get the index of the last element added to the VertexBiomeLocationsMap

  //BiomeLerping(YPos, XPos); //also blend with a neighbour biome to get good transitions
}//implement the biome blending to be within this function

void UBiomeGenerationComponent::BiomeBlending() //don't forget to include the terracing, if enabled
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_BiomeBlending);
#endif
	float StartAlpha = BlendAmount == 1 ? StartAlpha = 0.25f : StartAlpha = 0.4f;//FMath::Clamp(1 - 1.0f / (float)BlendAmount, 0.25f, 1.0f);
	//UE_LOG(LogTemp, Error, TEXT("The two biomes: %f"), InitBlend)

	for (int32 i = 0; i < TerrainGenerator->Vertices.Num(); i++)
	{
		int32 VertexX = FMath::RoundToInt(TerrainGenerator->Vertices[i].X / TerrainGenerator->GridSize);
		int32 VertexY = FMath::RoundToInt(TerrainGenerator->Vertices[i].Y / TerrainGenerator->GridSize);

		float OriginalZ = TerrainGenerator->Vertices[i].Z;

		TArray<int32> NeighbourBiomes;
		int32 CurrBlend = 1;
		//chech all neighbours of current vertex
		while (CurrBlend <= BlendAmount && BiomeStatsMap[BiomeAtEachPoint[i].Key].GetDefaultObject()->bDoBlending) //note, really inefficent and bad, just for quick testing
		{
			for (int32 Y = -CurrBlend; Y <= CurrBlend; Y++) //loop through all neighbouring grid points, rows
			{ //issue as while looping, will also check the same point a few times over
				for (int32 X = -CurrBlend; X <= CurrBlend; X++) //loop through all the columns
				{
					/*
						needs to actually go  -1 -1, 0, 1
											   0  0, x, 1
											   1  1, 0, 1

					*/


					//as long as theneighbour falls within the bounds of the island it can be lerped
					if (VertexY + Y >= 0 && VertexY + Y < TerrainGenerator->Height && VertexX + X >= 0 && VertexX + X < TerrainGenerator->Width)
					{
						//check the biome around the point(Y * Width + X)
						int32 NeighboursIndex = (VertexY + Y) * TerrainGenerator->Width + (VertexX + X); //this is the index of the value of the neighbouring currently checking
						float NeighbourValue = bBeenLerped[NeighboursIndex].Key ? bBeenLerped[NeighboursIndex].Value : TerrainGenerator->Vertices[NeighboursIndex].Z;
						if (BiomeAtEachPoint[i].Key != BiomeAtEachPoint[NeighboursIndex].Key
							&& !NeighbourBiomes.Contains(BiomeAtEachPoint[NeighboursIndex].Key)) //also not contain the same neighbour biome
						{
							int32 Dist = FMath::Clamp(FMath::Min(FMath::Abs(X), FMath::Abs(Y)), 1, 100);
							float Alpha = StartAlpha / (float)Dist;// -(float)((FMath::Max(FMath::Abs(X), FMath::Abs(Y)), 1, 100000) - 1) / (BlendAmount - 1.0f) * (0.5f);
							TerrainGenerator->Vertices[i].Z = FMath::Lerp(TerrainGenerator->Vertices[i].Z, NeighbourValue, Alpha);

							FLinearColor VertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[i].Key].GetDefaultObject()->BiomeColour; //placed here so will update with lerped colour
							FLinearColor NeighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighboursIndex].Key].GetDefaultObject()->BiomeColour;
							TerrainGenerator->VerticeColours[i] = FMath::Lerp(VertexBiomeColor, NeighbourBiomeColor, Alpha);

							NeighbourBiomes.Add(BiomeAtEachPoint[NeighboursIndex].Key);
						}
					}
				}
			}
			CurrBlend++;
		}
		bBeenLerped[i] = TPair<bool, float>(true, OriginalZ);
	}
}

void UBiomeGenerationComponent::BiomeLerping(int32 i, int32 j) //blend 2 neighbouring biome points together so smoother transition
{
	//int32 VertexIndex = i * TerrainGenerator->Width + j; //this is the current point at which is being checked
	//float VertexValue = TerrainGenerator->Vertices[VertexIndex].Z;
	//int32 blendAmount = 1; //how far away a neighbour can actually be

	////chech all neighbours of current vertex
	//for (int32 i1 = -blendAmount; i1 <= blendAmount; i1++) //loop through all neighbouring grid points
	//{
	//	for (int32 j1 = -blendAmount; j1 <= blendAmount; j1++)
	//	{
	//		//as long as theneighbour falls within the bounds of the island it can be lerped
	//		if (j + j1 >= 0 && j + j1 < TerrainGenerator->Width && i + i1 >= 0 && i + i1 < TerrainGenerator->Height)
	//		{
	//			//check the biome around the point
	//			int32 NeighbourIndex = (i + i1) * TerrainGenerator->Width + (j + j1); //this is the index of the value of the neighbouring currently checking
	//			float NeighbourValue = TerrainGenerator->Vertices[NeighbourIndex].Z;
	//			int32 CurrBiome = BiomeAtEachPoint[VertexIndex];
	//			if (BiomeAtEachPoint[VertexIndex] != BiomeAtEachPoint[NeighbourIndex] //if the two biomes are different, and are one of the onces which can actually lerp
	//				//&& BiomeAtEachPoint[VertexIndex] != 5 && BiomeAtEachPoint[NeighbourIndex] != 5
	//				//&& BiomeAtEachPoint[VertexIndex] != 6 && BiomeAtEachPoint[NeighbourIndex] != 6
	//				&& BiomeAtEachPoint[VertexIndex] != 2 && BiomeAtEachPoint[NeighbourIndex] != 2
	//				&& BiomeAtEachPoint[VertexIndex] != 3 && BiomeAtEachPoint[NeighbourIndex] != 3
	//				&& BiomeAtEachPoint[VertexIndex] != -1 && BiomeAtEachPoint[NeighbourIndex] != -1
	//				&& BiomeAtEachPoint[VertexIndex] != 14 && BiomeAtEachPoint[NeighbourIndex] != 14
	//				&& BiomeAtEachPoint[VertexIndex] != 15 && BiomeAtEachPoint[NeighbourIndex] != 15)
	//			{
	//				if (!bBeenLerped[VertexIndex].Key) //if this vertex has not yet been lerped
	//				{
	//					float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[NeighbourIndex].Z, VertexValue, 0.5f); //for vertex directly next to the new get value helpway between the 2
	//					//if (BiomeAtEachPoint[VertexIndex] == 1 || BiomeAtEachPoint[NeighbourIndex] == 1) //as next to ocean, update the biome to be a beach
	//					//{
	//					//	BiomeAtEachPoint[VertexIndex] = 13; //set the biome as a beach
	//					//	TerrainGenerator->VerticeColours[VertexIndex] = BiomeStatsMap[13].GetDefaultObject()->BiomeColour;
	//					//	TerrainGenerator->Vertices[VertexIndex].Z = WaterLine; //move Z height to be at the water
	//					//}
	//					//else //as not near ocean
	//					{
	//						TerrainGenerator->Vertices[VertexIndex].Z = LerpedValue; //set value of vertex to be the lerped one
	//						//get the biome colour of both points
	//						FLinearColor VertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[VertexIndex]].GetDefaultObject()->BiomeColour;
	//						FLinearColor NeighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].GetDefaultObject()->BiomeColour;
	//						TerrainGenerator->VerticeColours[VertexIndex] = FMath::Lerp(VertexBiomeColor, NeighbourBiomeColor, 0.25f); //blend the colours, using the Vertex as the main colour
	//					}
	//					bBeenLerped[VertexIndex] = TPair<bool, float>(true, -1); //update the value so can no longer be blended again
	//				}
	//				if (!bBeenLerped[NeighbourIndex].Key) //repeate the above, but for the neighbours vertex, if it hasn't yet been lerped
	//				{
	//					float LerpedValue = FMath::Lerp(TerrainGenerator->Vertices[NeighbourIndex].Z, VertexValue, 0.5f); //for vertex directly next to the new biome
	//					if (BiomeAtEachPoint[NeighbourIndex] == 1 || BiomeAtEachPoint[VertexIndex] == 1)
	//					{
	//						BiomeAtEachPoint[NeighbourIndex] = 13; //set the biome as a beach
	//						TerrainGenerator->VerticeColours[NeighbourIndex] = BiomeStatsMap[13].GetDefaultObject()->BiomeColour;
	//						TerrainGenerator->Vertices[NeighbourIndex].Z = WaterLine;
	//					}
	//					else
	//					{
	//						TerrainGenerator->Vertices[NeighbourIndex].Z = LerpedValue;
	//						FLinearColor VertexBiomeColor = BiomeStatsMap[BiomeAtEachPoint[VertexIndex]].GetDefaultObject()->BiomeColour;
	//						FLinearColor NeighbourBiomeColor = BiomeStatsMap[BiomeAtEachPoint[NeighbourIndex]].GetDefaultObject()->BiomeColour;
	//						TerrainGenerator->VerticeColours[NeighbourIndex] = FMath::Lerp(VertexBiomeColor, NeighbourBiomeColor, 0.75f);
	//					}
	//					bBeenLerped[NeighbourIndex] = TPair<bool, float>(true, -1);
	//				}
	//			}
	//		}
	//	}
	//}
}

void UBiomeGenerationComponent::SpawnStructure()
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_SpawnStructures);
#endif

	AStaticMeshActor* SpawnedTentMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	SpawnedTentMesh->NetDormancy = ENetDormancy::DORM_DormantAll;
	SpawnedTentMesh->SetMobility(EComponentMobility::Stationary);

	MeshActors.Add(SpawnedTentMesh); //add the mesh to the list of all meshes within the map

	UInstancedStaticMeshComponent* InstancedTentMesh = NewObject<UInstancedStaticMeshComponent>(SpawnedTentMesh);
	InstancedTentMesh->RegisterComponent();
	SpawnedTentMesh->AddInstanceComponent(InstancedTentMesh);
	InstancedTentMesh->SetStaticMesh(Tent); //assign the appropriate mesh

	AStaticMeshActor* SpawnedBouyMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	SpawnedBouyMesh->NetDormancy = ENetDormancy::DORM_DormantAll;
	SpawnedBouyMesh->SetMobility(EComponentMobility::Stationary);

	MeshActors.Add(SpawnedBouyMesh); //add the mesh to the list of all meshes within the map

	UInstancedStaticMeshComponent* InstancedBouyMesh = NewObject<UInstancedStaticMeshComponent>(SpawnedBouyMesh);
	InstancedBouyMesh->RegisterComponent();
	SpawnedBouyMesh->AddInstanceComponent(InstancedBouyMesh);
	InstancedBouyMesh->SetStaticMesh(Bouy); //assign the appropriate mesh


	int32 StructureAmount = FMath::CeilToInt(TerrainGenerator->Width * TerrainGenerator->Height / 1500.0f); //determine number to spawn in based on map size, rounding if ends up being a float
	//* divide by 1500 so that if say have a 100 by 100 map will have 6 tents spawn on it
	if (!TerrainGenerator->bIsEditor && GetWorld()->IsServer())
	{//update the servers game state to hold total number of structures needed to collect
		AMainGameState* MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
		MainGameState->MaxFuelAmount = StructureAmount;
	}

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
	} //using structure amount 3 times here, why?

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
		FTransform InstancedMeshTransform;

		//Assign the appropriate transform
		InstancedMeshTransform.SetLocation(VertexLocation);

		if (GetWorld()->IsServer() || TerrainGenerator->bIsEditor) //only spawn spawners/fuel in on the server version
		{
			SpawnZombieSpawner(VertexLocation, VertexIndex);

			FVector FuelLocation = VertexLocation;
			FuelLocation.Z += 200; //apply offset to the fuels location
			AFuelPellet* FuelPellet = GetWorld()->SpawnActor<AFuelPellet>(TerrainGenerator->Fuel, FuelLocation, FRotator::ZeroRotator); //spawn in fuel at the mesh
			MeshActors.Add(FuelPellet);
		}

		if (TerrainGenerator->Vertices[VertexIndex].Z < 0) //spawn in a bouy as must be underwater
		{
			InstancedMeshTransform.SetScale3D(FVector(10)); //give the bouy a certain scale
			InstancedBouyMesh->AddInstanceWorldSpace(InstancedMeshTransform);
		}
		else //as on land spawn in a tent
		{
			InstancedMeshTransform.SetScale3D(FVector(40)); //give the bouy a certain scale
			InstancedTentMesh->AddInstanceWorldSpace(InstancedMeshTransform); //assign the appropriate mesh to the spawned in actor
		}

		//add the meshes to the list of all meshes within the map so will be destroyed when resetting map
		GridPoints[RandomIndex] = GridPoints[GridPoints.Num() - 1];
		GridPoints.RemoveAt(GridPoints.Num() - 1);
	}
}


void UBiomeGenerationComponent::SpawnMeshes() //spawn in the plants into the map
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_SpawnMeshes);
#endif
	//really performance heavy
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


				AStaticMeshActor* SpawnedMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
				SpawnedMesh->NetDormancy = ENetDormancy::DORM_DormantAll;
				SpawnedMesh->SetMobility(EComponentMobility::Stationary);
				SpawnedMesh->SetActorEnableCollision(DifferentMeshes.bHasCollision);

				MeshActors.Add(SpawnedMesh); //add the mesh to the list of all meshes within the map

				UInstancedStaticMeshComponent* InstancedMesh = NewObject<UInstancedStaticMeshComponent>(SpawnedMesh);
				InstancedMesh->RegisterComponent();
				SpawnedMesh->AddInstanceComponent(InstancedMesh);
				InstancedMesh->SetStaticMesh(DifferentMeshes.Mesh); //assign the appropriate mesh
				if(!DifferentMeshes.bHasCollision)
					InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); //update the meshes collision so if say grass it will not have any player collision


				while (MeshesAdded < MeshesDensity) //spawn for the determined desnity of the mesh spawn that many into the map
				{
#if WITH_EDITOR
					SCOPE_CYCLE_COUNTER(STAT_LoopMesh);
#endif
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
#if WITH_EDITOR
							SCOPE_CYCLE_COUNTER(STAT_LoopMesh1);
#endif
							for (int32 a = -DifferentMeshes.NeighbourRadius; a < DifferentMeshes.NeighbourRadius; a++) //add all points within a 2 radius of the choosen one
							{
								for (int32 b = -DifferentMeshes.NeighbourRadius; b < DifferentMeshes.NeighbourRadius; b++)
								{
									if (XCentre + b >= 0 && XCentre + b < TerrainGenerator->Width && YCentre + a >= 0 && YCentre + a < TerrainGenerator->Height)
									{ //as long as the point is actually on the map
										int32 NeighbourIndex = (a + YCentre) * TerrainGenerator->Width + (b + XCentre);
										if (BiomeAtEachPoint[NeighbourIndex].Value != -1 && BiomePoints.Key == BiomeAtEachPoint[NeighbourIndex].Key) 
											//ensure neighbour does not contain a mesh and will be the same biome
										{
#if WITH_EDITOR
											SCOPE_CYCLE_COUNTER(STAT_BiomePoint);
#endif

											//now can actually add the point to the radius, as long as exists so doesn't contain another mesh
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
#if WITH_EDITOR
							SCOPE_CYCLE_COUNTER(STAT_LoopMesh2);
#endif
							RandomLocation = TerrainGenerator->Stream.RandRange(0, RadiusPoints.Num() - 1);
							if (BiomePoints.Value.Num() > 0 && RadiusPoints.Num() > 0 && MeshesAdded <= MeshesDensity)
							{
								FTransform InstancedMeshTransform;

								VertexIndex = RadiusPoints[RandomLocation];
								FVector VertexLocation = MeshLocation(TerrainGenerator->Vertices[VertexIndex]); //adjust the location so somewhat offset

								InstancedMeshTransform.SetLocation(VertexLocation);

								FRotator Rotation = FRotator(0, 0, 0); //give the mesh a random Yaw rotation
								Rotation.Yaw = TerrainGenerator->Stream.FRandRange(0.0f, 360.0f);

								InstancedMeshTransform.Rotator() = Rotation;
								////spawn in a new Actor in specified location, with random rotation

								InstancedMeshTransform.SetScale3D(FVector(TerrainGenerator->Stream.FRandRange(15.0f, 45.0f)));
								InstancedMesh->AddInstanceWorldSpace(InstancedMeshTransform);

								{ //////these two lines of code here really bad performance, taking around 12 seconds in total
#if WITH_EDITOR
									SCOPE_CYCLE_COUNTER(STAT_AddToWorld);
#endif
									//remove the choosen location from the list so no new meshes can spawn there

									//need to ensure getting the index of the point on the BiomePoints array somehow
									int32 PointIndex = BiomeAtEachPoint[VertexIndex].Value; //the position of the element removing within the BiomePoints Array
									int32 LastIndex = BiomePoints.Value.Num() - 1;
									BiomeAtEachPoint[BiomePoints.Value[LastIndex]].Value = PointIndex; //get the value as seen in the Biome of point array, updating it so it is now linking to its new location
									BiomeAtEachPoint[VertexIndex].Value = -1; //tree so make it empty

									BiomePoints.Value[PointIndex] = BiomePoints.Value[LastIndex]; //somehow one of these values is wrong  //swap value at vertex index and last value
									BiomePoints.Value.RemoveAt(LastIndex); //swap with the last index in the list and then remove it so gets o(1)
									
									RadiusPoints[RandomLocation] = RadiusPoints[RadiusPoints.Num() - 1];
									RadiusPoints.RemoveAt(RadiusPoints.Num() - 1);
								}

								MeshesAdded++;
							}
							else
								break;
						}
					}
				}
			}
		}
	}
}

FVector UBiomeGenerationComponent::MeshLocation(FVector VertexPosition) //in a square around the vertex spawning at, randomly place the mesh, so not appearing as a grid like pattern
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_MeshLocation);
#endif
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
	VertexPosition.X = FMath::Clamp(RandXPosition, (float)-(TerrainGenerator->Width- 1) * TerrainGenerator->GridSize, (float)(TerrainGenerator->Width - 1) * TerrainGenerator->GridSize);
	VertexPosition.Y = FMath::Clamp(RandYPosition, (float)-(TerrainGenerator->Height - 1) * TerrainGenerator->GridSize, (float)(TerrainGenerator->Height - 1) * TerrainGenerator->GridSize);
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

	int32 BiomeKey = BiomeAtEachPoint[Index].Key; //get the biome of the current point
	ZombieSpawner->ZombieScale = BiomeStatsMap[BiomeKey].GetDefaultObject()->Scale;
	ZombieSpawner->ZombieSwimSpeed = BiomeStatsMap[BiomeKey].GetDefaultObject()->SwimSpeed;
	ZombieSpawner->ZombieWalkSpeed = BiomeStatsMap[BiomeKey].GetDefaultObject()->WalkSpeed;

	MeshActors.Add(ZombieSpawner);
}
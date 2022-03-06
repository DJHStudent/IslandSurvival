// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedurallyGeneratedTerrain.h"
#include "KismetProceduralMeshLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "TimerManager.h"
#if WITH_EDITOR
//stats to track
DECLARE_CYCLE_STAT(TEXT("Create Mesh Details"), STAT_CreateMesh, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Create Mesh Vertices"), STAT_CreateVertices, STATGROUP_ProcedurallyGeneratedTerrain);
DECLARE_CYCLE_STAT(TEXT("Inital Height"), STAT_InitialHeight, STATGROUP_ProcedurallyGeneratedTerrain);
#endif

// Sets default values
AProcedurallyGeneratedTerrain::AProcedurallyGeneratedTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BiomeGeneration = CreateDefaultSubobject<UBiomeGenerationComponent>("Biome Generation Component"); //create a new component for handling biomes
	if (BiomeGeneration)
	{
		BiomeGeneration->TerrainGenerator = this;
		BiomeGeneration->SetIsReplicated(true);
	}

	//Set the default values for each paramter
	Width = 100;
	Height = 100;
	GridSize = 200;

	Seed = 0;
	bRandomSeed = false;
	bSmoothTerrain = false;
	bReplicates = true;
	bIsEditor = false;

	ChunkSize = 100;
}

// Called when the game starts or when spawned
void AProcedurallyGeneratedTerrain::BeginPlay()
{
	Super::BeginPlay();
}

bool AProcedurallyGeneratedTerrain::ShouldTickIfViewportsOnly() const //run the code within the viewport when not running
{
	return true;
}
// Called every frame
void AProcedurallyGeneratedTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//when the vertices array is completed and is not null
	if (AsyncVertices && AsyncVertices->IsDone())
	{
		AsyncVertices->EnsureCompletion();
		delete AsyncVertices;
		AsyncVertices = nullptr;
		GenerateMeshes(); //spawn in the plants etc
	}
}

void AProcedurallyGeneratedTerrain::GenerateNewTerrain()
{
	if (!bIsEditor) //if in editor and true regenerate map
	{
		if (bRandomSeed)
		{
			Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
			Seed = Stream.GetCurrentSeed(); //assign the seed the streams seed
		}
		else
		{
			Stream.Initialize(Seed);
			Seed = Seed;
		}
		bIsEditor = true;
		RegenerateMap(Seed, Width, Height, Stream, bSmoothTerrain);
	}
}

void AProcedurallyGeneratedTerrain::RegenerateMap(int32 tSeed, int32 tWidth, int32 tHeight, FRandomStream tStream, bool tbSmoothTerrain)
{
	this->Seed = tSeed;
	this->Width = tWidth;
	this->Height = tHeight;
	this->Stream = tStream;
	this->bSmoothTerrain = tbSmoothTerrain;

	RegenContinued();
}

void AProcedurallyGeneratedTerrain::RegenContinued()
{
	ClearMap(); //delete any previosuly stored values

	GenerateSeed(); //determine seed
	if (TerrainHeight)
	{
		TerrainHeight->Width = Width;
		TerrainHeight->Height = Height;
	}
	//CreateMesh(); //generate the terrain mesh
	AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	AsyncVertices = (new FAsyncTask<AsyncTerrainGeneration>(ProceduralTerrain));
	AsyncVertices->StartBackgroundTask();
}

void AProcedurallyGeneratedTerrain::ClearMap() //empties the map removing all data for it
{
	//empty all generated arrays
	Vertices.Empty();
	VerticeColours.Empty();
	FeatureNumber.Empty();
	
	ChunkTriangles.Empty();
	if (BiomeGeneration)
	{
		//reset all info on biomes
		BiomeGeneration->FeatureKeys = 0;
		BiomeGeneration->FeaturePointsMap.Empty();

		BiomeGeneration->SingleLakeBiomeKeys.Empty();
		BiomeGeneration->SingleLandBiomeKeys.Empty();		
		BiomeGeneration->MultiLakeBiomeKeys.Empty();
		BiomeGeneration->MultiLandBiomeKeys.Empty();
		BiomeGeneration->HeightBiomeKeys.Empty();

		BiomeGeneration->BiomeAtEachPoint.Empty();
		BiomeGeneration->VertexBiomeLocationsMap.Empty();
		BiomeGeneration->bBeenLerped.Empty();


		//destory any meshes placed on the terrain by using the array of all meshes which exist
		for (int32 i = BiomeGeneration->MeshActors.Num() - 1; i >= 0; i--)
		{
			if (BiomeGeneration->MeshActors[i])
				BiomeGeneration->MeshActors[i]->Destroy();
		}
		BiomeGeneration->MeshActors.Empty();
	}
}

void AProcedurallyGeneratedTerrain::GenerateSeed() //give a random seed, otherwise use the specified one from editor
{
	if (BiomeGeneration) 
	{
		for (auto& BiomeStats : BiomeGeneration->BiomeStatsMap) //for each biome give a random offset for each noise value to use
		{ //move this to a more appropriate place i.e when calling noise function for mesh I guess
			if (BiomeStats.Value.GetDefaultObject()->TerrainHeight)
				BiomeStats.Value.GetDefaultObject()->TerrainHeight->DeclareOffsetValues(Stream);
		}
	}
	if(TerrainHeight)
		TerrainHeight->DeclareOffsetValues(Stream);
}

void AProcedurallyGeneratedTerrain::CreateMesh() //make the map generate populating all the nessesary data
{
#if WITH_EDITOR
	SCOPE_CYCLE_COUNTER(STAT_CreateMesh);
#endif
	{
#if WITH_EDITOR
		SCOPE_CYCLE_COUNTER(STAT_CreateVertices);
#endif
		//loop through each vertex of the terrain
		for (int32 i = 0; i < Height; i++) //can this be deivided up somehow so parts of it can run async
		{ //least performant part of the entire list now
			for (int32 j = 0; j < Width; j++)
			{
				float ZPosition;
				{
#if WITH_EDITOR
					SCOPE_CYCLE_COUNTER(STAT_InitialHeight);
#endif
					ZPosition = TerrainHeight->GenerateHeight(j, i, BiomeGeneration->WaterLine, bSmoothTerrain); //get the specific height for each point on the mesh
				}
				Vertices.Add(FVector(j * GridSize, i * GridSize, ZPosition));

				if (BiomeGeneration)
				{
					BiomeGeneration->AddBiomePoints(j, i, ZPosition); //Calculate the feature this point relates to for the biome generation

					//initilize the two arrays with default values
					BiomeGeneration->BiomeAtEachPoint.Add(TPair<int32, int32>(-1, -1)); //give each vertex a default empty biome
					BiomeGeneration->bBeenLerped.Add(TPair<bool, float>(false, -1));
				}
				VerticeColours.Add(FLinearColor(1, 1, 1)); //give each vertex a default colour of white
			}
		}
	}
	if (!bOverrideBiomeSpawning)
	{
		BiomeGeneration->EachPointsMap();//determine the biome of each vertex of the map
		BiomeGeneration->BiomeBlending(); //make work by running it as soon as the vertice has a biome choosen
	}
}

void AProcedurallyGeneratedTerrain::GenerateMeshes() //make the map generate populating all the nessesary data
{
	if (!bOverrideBiomeSpawning)
	{
		BiomeGeneration->SpawnStructure();
		BiomeGeneration->SpawnMeshes(); //spawn in all the appropriate meshes for each biome
	}
	//either this or find the max number which can divide evenly into both the width and height
	int32 ChunkWidth = ChunkSize > Width ? Width : ChunkSize;//FMath::CeilToInt(Width / (FMath::CeilToInt(Width / (float)ChunkSize)));
	int32 ChunkHeight = ChunkSize > Height ? Height : ChunkSize;//FMath::CeilToInt(Width / (FMath::CeilToInt(Width / (float)ChunkSize)));
	int32 ChunkXAmount = FMath::CeilToInt(Width / (float)ChunkWidth);
	int32 ChunkYAmount = FMath::CeilToInt(Height / (float)ChunkWidth);
	SpawnChunk(0, 0, ChunkXAmount, ChunkYAmount, ChunkWidth, ChunkHeight);

	if (!bIsEditor)
	{
		UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (MainGameInstance)
			MainGameInstance->FinishTerrainLoading();
	}
	bIsEditor = false;
}

void AProcedurallyGeneratedTerrain::SpawnChunk(int32 i, int32 j, const int32 ChunkXAmount, const int32 ChunkYAmount, int32 ChunkWidth, int32 ChunkHeight)
{
	//UE_LOG(LogTemp, Warning, TEXT("Making a new chunk: %i"), ChunkAmount)
	TArray<FVector> ChunkVertices;
	TArray<FLinearColor> ChunkColours;

	TArray<int32> SmallerChunkTriangles;

	//actual width and height of the current chunk
	//have 3 values: width of terrain, 
	int32 CurrChunkWidth = FMath::Clamp(ChunkWidth * (j + 1) + 1, 0, Width) - j * ChunkWidth;
	int32 CurrChunkHeight = FMath::Clamp(ChunkHeight * (i + 1) + 1, 0, Height) - i * ChunkHeight;
	if (CurrChunkHeight > 1 && CurrChunkWidth > 1)
	{
		for (int32 Y = FMath::Clamp(i * ChunkHeight, 0, 10000000); Y < FMath::Clamp(ChunkHeight * (i + 1) + 1, 0, Height); Y++) //loop through all vertices of the chunk
		{
			//for the actual width of a chunk, need to minus width by the current x position
			for (int32 X = FMath::Clamp(j * ChunkWidth, 0, 100000000); X < FMath::Clamp(ChunkWidth * (j + 1) + 1, 0, Width); X++)
			{
				int32 Index = Y * Width + X;

				if (Y < Height && X < Width)
				{
					ChunkVertices.Add(Vertices[Index]);
					if (i == 0 && j == 0) //Setup triangles for a chunk so they don't need to be calculated for each(if same)
					{
						if (Y + 1 < ChunkHeight + 1 && X + 1 < ChunkWidth + 1) //add the appropriate triangles in the right positions within the array
						{
							ChunkTriangles.Add(Y * (ChunkWidth + 1) + X); ChunkTriangles.Add((Y + 1) * (ChunkWidth + 1) + X); ChunkTriangles.Add(Y * (ChunkWidth + 1) + (X + 1));
							ChunkTriangles.Add(Y * (ChunkWidth + 1) + (X + 1)); ChunkTriangles.Add((Y + 1) * (ChunkWidth + 1) + X); ChunkTriangles.Add((Y + 1) * (ChunkWidth + 1) + (X + 1));
						}
					}
					//do when width and height different

					if (CurrChunkWidth != ChunkWidth + 1 || CurrChunkHeight != ChunkHeight + 1 || j == ChunkXAmount - 1 || i == ChunkYAmount - 1)
					{
						int32 TX = X - (j * ChunkWidth);
						int32 TY = Y - (i * ChunkHeight);

						if (TY + 1 < CurrChunkHeight && TX + 1 < CurrChunkWidth) //add the appropriate triangles in the right positions within the array
						{
							SmallerChunkTriangles.Add(TY * (CurrChunkWidth)+TX); SmallerChunkTriangles.Add((TY + 1) * (CurrChunkWidth)+TX); SmallerChunkTriangles.Add(TY * (CurrChunkWidth)+(TX + 1));
							SmallerChunkTriangles.Add(TY * (CurrChunkWidth)+(TX + 1)); SmallerChunkTriangles.Add((TY + 1) * (CurrChunkWidth)+TX); SmallerChunkTriangles.Add((TY + 1) * (CurrChunkWidth)+(TX + 1));
						}
					}

					//if the chunk size ends up having a width less than the specified one, due to terrains size given chunk triangles will be too large
					ChunkColours.Add(VerticeColours[Index]);
				}
			}
		}
		//UE_LOG(LogTemp, Error, TEXT("Triangle List Size: %i, %i"), ChunkTriangles.Num(), ChunkVertices.Num());
		AActor* MeshChunk = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		UProceduralMeshComponent* ProceduralChunk = NewObject<UProceduralMeshComponent>(MeshChunk);
		ProceduralChunk->SetIsReplicated(true);
		ProceduralChunk->RegisterComponent();
		MeshChunk->AddInstanceComponent(ProceduralChunk);
		if (TerrainMaterial)
			ProceduralChunk->SetMaterial(0, TerrainMaterial);

		BiomeGeneration->MeshActors.Add(MeshChunk);
		//UE_LOG(LogTemp, Warning, TEXT("Chunk Sizes: %i, %i, %i, %i, %i, %i"), CurrChunkHeight, CurrChunkWidth, ChunkHeight, ChunkWidth, ChunkXAmount, ChunkYAmount)
		if (SmallerChunkTriangles.Num() > 0)
			ProceduralChunk->CreateMeshSection_LinearColor(int32(0), ChunkVertices, SmallerChunkTriangles, TArray<FVector>(), TArray<FVector2D>(), ChunkColours, TArray<FProcMeshTangent>(), true);
		else
			ProceduralChunk->CreateMeshSection_LinearColor(int32(0), ChunkVertices, ChunkTriangles, TArray<FVector>(), TArray<FVector2D>(), ChunkColours, TArray<FProcMeshTangent>(), true);
	}
	if (j + 1 < ChunkXAmount)
	{
		j++;
		//spawn the next chunk, one column down
		//SpawnChunk(i, j, ChunkAmount, ChunkWidth);
		FTimerDelegate WaitDelegate = FTimerDelegate::CreateUObject(this, &AProcedurallyGeneratedTerrain::SpawnChunk, i, j, ChunkXAmount, ChunkYAmount, ChunkWidth, ChunkHeight);
		GetWorld()->GetTimerManager().SetTimerForNextTick(WaitDelegate);//wait for frame to end and next one to start
	}
	else if (i + 1 < ChunkYAmount)
	{
		j = 0;
		i++;
		//spawn next chunk, one row down
		//SpawnChunk(i, j, ChunkAmount, ChunkWidth);

		FTimerDelegate WaitDelegate = FTimerDelegate::CreateUObject(this, &AProcedurallyGeneratedTerrain::SpawnChunk, i, j, ChunkXAmount, ChunkYAmount, ChunkWidth, ChunkHeight);
		GetWorld()->GetTimerManager().SetTimerForNextTick(WaitDelegate);//wait for frame to end and next one to start
	}
}
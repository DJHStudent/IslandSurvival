// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedurallyGeneratedTerrain.h"
#include "KismetProceduralMeshLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProcedurallyGeneratedTerrain::AProcedurallyGeneratedTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //create a procedural mesh for the terrain
	if (MeshComponent)
		MeshComponent->SetIsReplicated(true);
	BiomeGeneration = CreateDefaultSubobject<UBiomeGenerationComponent>("Biome Generation Component"); //create a new component for handling biomes
	if (BiomeGeneration)
	{
		BiomeGeneration->TerrainGenerator = this;
		BiomeGeneration->SetIsReplicated(true);
	}
	//TerrainHeight = NewObject<UTerrainHeight>(this, TEXT("Terrain Height"));//, EObjectFlags::RF_Public);//NewObject<UTerrainHeight>();

	//Set the default values for each paramter
	Width = 100;
	Height = 100;
	GridSize = 200;
//	PerlinScale = 25000;
//	PerlinRoughness = 0.01f;

	Seed = 0;
	bRandomSeed = false;

	bReplicates = true;

//	Octaves = 8;
//	Lacunarity = 2.1f;
//	Grain = 0.5f;

//	DomainAmount = 40;

//	Size = 2.0f;
//	Steepness = 20.0f;
//	AboveWater = 0.0025f;

//	TerraceSize = 112.5f;
}

// Called when the game starts or when spawned
void AProcedurallyGeneratedTerrain::BeginPlay()
{
	Super::BeginPlay();

	//make a new map when generating in the world
	//
	//if (GameState)
	//{
	//	Width = GameState->TerrainWidth;
	//	Height = GameState->TerrainHeight;
	//	Seed = GameState->TerrainSeed;
	//	Stream = GameState->Stream;
	//}
	//RegenerateMap();
	GameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
	SpawnMap();
}

void AProcedurallyGeneratedTerrain::SpawnMap()
{
	if (GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("Waiting for Game to begin"))
		if (GameState->bStreamRep && GameState->bSeedRep && GameState->bWidthRep && GameState->bHeightRep) //if all values properly replicated
			RegenerateMap(); //update the map
		else //otherwise wait some time and try updating map again
		{
			float RepWaitTime = 1.0f;
			FTimerHandle Timer; //timer to handle spawning of player after death
			GetWorldTimerManager().SetTimer(Timer, this, &AProcedurallyGeneratedTerrain::SpawnMap, RepWaitTime, false);
		}
	}
}

bool AProcedurallyGeneratedTerrain::ShouldTickIfViewportsOnly() const //run the code within the viewport when not running
{
	return true;
}
// Called every frame
void AProcedurallyGeneratedTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMap) //if in editor and true regenerate map
	{
		//RegenerateMap();
	}
}

void AProcedurallyGeneratedTerrain::RegenerateMap()
{
	UE_LOG(LogTemp, Warning, TEXT("Terrain Generated Succesfully"))
	if (GameState)
	{
		Width = GameState->TerrainWidth;
		Height = GameState->TerrainHeight;
		Stream = GameState->Stream;
		Seed = GameState->TerrainSeed;
	}

	ClearMap(); //delete any previosuly stored values

	if (BiomeGeneration)
	{
		BiomeGeneration->BiomeAtEachPoint.Init(-1, Width * Height); //give each vertex a default biome of ocean
		BiomeGeneration->bBeenLerped.Init(false, Width * Height);
	}
	VerticeColours.Init(FLinearColor(1, 1, 1), Width * Height); //give each vertex a default colour

	GenerateSeed(); //determine seed
	if (TerrainHeight)
	{
		TerrainHeight->Width = Width;
		TerrainHeight->Height = Height;
	}
	CreateMesh(); //generate the terrain mesh
	bRegenerateMap = false;
}

void AProcedurallyGeneratedTerrain::ClearMap() //empties the map removing all data for it
{
	//empty all generated arrays
	Vertices.Empty();
	VerticeColours.Empty();
	IslandNumber.Empty();

	Triangles.Empty();

	if (BiomeGeneration)
	{
		//reset all info on biomes
		BiomeGeneration->IslandKeys = 0;
		BiomeGeneration->IslandPointsMap.Empty();
		BiomeGeneration->BiomeAtEachPoint.Empty();
		BiomeGeneration->VertexBiomeLocationsMap.Empty();
		BiomeGeneration->bBeenLerped.Empty();
	}

	//destory any meshes placed on the terrain by using the array of all meshes which exist
	for (int32 i = BiomeGeneration->MeshActors.Num() - 1; i >= 0 ; i--)
	{
		if(BiomeGeneration->MeshActors[i])
			BiomeGeneration->MeshActors[i]->Destroy();
	}
	BiomeGeneration->MeshActors.Empty();

	MeshComponent->ClearAllMeshSections(); //removes all mesh sections, returning it to empty state
}

void AProcedurallyGeneratedTerrain::GenerateSeed() //give a random seed, otherwise use the specified one from editor
{
	////if (bRandomSeed || Seed == 0) //if user has requested a random seed
	////{
	////	Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
	////	Seed = Stream.GetCurrentSeed(); //assign the seed the streams seed
	////}
	////else
	////	Stream.Initialize(Seed); //initilizes the RNG with a specific seed

	if (BiomeGeneration) 
	{
		for (auto& BiomeStats : BiomeGeneration->BiomeStatsMap) //for each biome give a random offset for each noise value to use
		{
			if (BiomeStats.Value.GetDefaultObject()->TerrainHeight)
				BiomeStats.Value.GetDefaultObject()->TerrainHeight->DeclareOffsetValues(Stream);
		}
	}
	if(TerrainHeight)
		TerrainHeight->DeclareOffsetValues(Stream);
}

void AProcedurallyGeneratedTerrain::CreateMesh() //make the map generate populating all the nessesary data
{
	//loop through each vertex of the terrain
	for (int32 i = 0; i < Height; i++)
	{
		for (int32 j = 0; j < Width; j++)
		{
			float ZPosition = TerrainHeight->GenerateHeight(j, i); //get the specific height for each point on the mesh
			BiomeGeneration->AddIslandPoint(j, i, ZPosition); //Calculate the island this point relates to for the biome generation

			Vertices.Add(FVector(j * GridSize, i * GridSize, ZPosition)); 

			if (i + 1 < Height && j + 1 < Width) //add the appropriate triangles in the right positions within the array
			{
				Triangles.Add(i * Width + j); Triangles.Add((i + 1) * Width + j); Triangles.Add(i * Width + (j + 1));
				Triangles.Add(i * Width + (j + 1)); Triangles.Add((i + 1) * Width + j); Triangles.Add((i + 1) * Width + (j + 1));
			}
		}
	}
	BiomeGeneration->VerticesBiomes();//determine the biome of each vertex of the map which is above water
	//BiomeGeneration->BiomeLerping();
	BiomeGeneration->SpawnTents();
	BiomeGeneration->SpawnMeshes(); //spawn in all the appropriate meshes for each biome

	//generate the terrain with the specified colour and do collision, and normals caculated on the material
	MeshComponent->CreateMeshSection_LinearColor(int32(0), Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), VerticeColours, TArray<FProcMeshTangent>(), true);
}

void AProcedurallyGeneratedTerrain::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	////////DOREPLIFETIME(AProcedurallyGeneratedTerrain, Seed);
	////////DOREPLIFETIME(AProcedurallyGeneratedTerrain, Stream);
	////////DOREPLIFETIME(AProcedurallyGeneratedTerrain, Width);
	////////DOREPLIFETIME(AProcedurallyGeneratedTerrain, Height);
}
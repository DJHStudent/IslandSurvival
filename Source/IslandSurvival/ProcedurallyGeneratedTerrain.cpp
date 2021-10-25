// Fill out your copyright notice in the Description page of Project Settings.


#include "ProcedurallyGeneratedTerrain.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AProcedurallyGeneratedTerrain::AProcedurallyGeneratedTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //create a procedural mesh for the terrain

	BiomeGeneration = CreateDefaultSubobject<UBiomeGenerationComponent>("Biome Generation Component"); //create a new component for handling biomes
	BiomeGeneration->TerrainGenerator = this;

	//TerrainHeight = NewObject<UTerrainHeight>(this, TEXT("Terrain Height"));//, EObjectFlags::RF_Public);//NewObject<UTerrainHeight>();

	//Set the default values for each paramter
	Width = 100;
	Height = 100;
	GridSize = 200;
//	PerlinScale = 25000;
//	PerlinRoughness = 0.01f;

	Seed = 0;
	bRandomSeed = false;

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
	RegenerateMap();
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
		RegenerateMap();
	}
}

void AProcedurallyGeneratedTerrain::RegenerateMap()
{
	ClearMap(); //delete any previosuly stored values

	if(BiomeGeneration)
		BiomeGeneration->BiomeAtEachPoint.Init(1, Width * Height); //give each vertex a default biome of ocean
	VerticeColours.Init(FLinearColor(1, 1, 1), Width * Height); //give each vertex a default colour

	GenerateSeed(); //determine seed
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

	OcataveOffsets.Empty();

	//reset all info on biomes
	BiomeGeneration->IslandKeys = 0;
	BiomeGeneration->IslandPointsMap.Empty();
	BiomeGeneration->BiomeAtEachPoint.Empty();
	BiomeGeneration->VertexBiomeLocationsMap.Empty();

	//destory any meshes placed on the terrain by using the array of all meshes which exist
	for (int32 i = BiomeGeneration->MeshActors.Num() - 1; i >= 0 ; i--)
	{
		BiomeGeneration->MeshActors[i]->Destroy();
	}
	BiomeGeneration->MeshActors.Empty();

	MeshComponent->ClearAllMeshSections(); //removes all mesh sections, returning it to empty state
}

void AProcedurallyGeneratedTerrain::GenerateSeed() //give a random seed, otherwise use the specified one from editor
{
	if (bRandomSeed || Seed == 0) //if user has requested a random seed
	{
		Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
		Seed = Stream.GetCurrentSeed(); //assign the seed the streams seed
	}
	else
		Stream.Initialize(Seed); //initilizes the RNG with a specific seed

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
	BiomeGeneration->BiomeLerping();
	BiomeGeneration->SpawnMeshes(); //spawn in all the appropriate meshes for each biome

	//generate the terrain with the specified colour and do collision, and normals caculated on the material
	MeshComponent->CreateMeshSection_LinearColor(int32(0), Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), VerticeColours, TArray<FProcMeshTangent>(), true);
}

////////float AProcedurallyGeneratedTerrain::FractalBrownianMotion(int32 XPosition, int32 YPosition)
////////{
////////	float HeightSum = 0; //the sum of the height at each octave
////////	float Frequency = 1; //offset value for steepness of each successive octave
////////	float Amplitude = 1; //the offset value for the height at each octave
////////
////////	for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
////////	{
////////		//for each octave determine the noise value to use, using frequency
////////		float NoiseValue = (FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffsets[i], YPosition + OcataveOffsets[i]) * Frequency * PerlinRoughness));
////////		//NoiseValue = (NoiseValue - -1) / (1 - -1) * (1 - 0) + 0; //normalize value into range of 0 - 1
////////	
////////		HeightSum += NoiseValue * Amplitude; //add this noise value to the total with amplitude
////////
////////		Frequency *= Lacunarity; //the influence of the frequency over each sucessive octave, increasing
////////		Amplitude *= Grain; //influence of amplitude on each sucessive octave, decreasing
////////	}
////////	return HeightSum; //return the final height sum
////////}
////////
////////
////////float AProcedurallyGeneratedTerrain::DomainWarping(float XPosition, float YPosition) //for each vertex offset its height by a specific amount of values, through combining multiple FBM noise
////////{
////////	//calculate the firest points X and Y position
////////	FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
////////	//determine the next points X and Y position based on q's point values
////////	FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));
////////
////////	//Determine the final new height to give the point
////////	float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);
////////
////////	return NewHeight;
////////}
////////
////////
////////float AProcedurallyGeneratedTerrain::GenerateHeight(int32 XPosition, int32 YPosition) //all the functions for determining the height of a specific point
////////{
////////	float FBMValue = DomainWarping(XPosition, YPosition);//+ 1) / 2; //determine the inital value of the point using domain warping
////////
////////	float HeightValue = FBMValue;
////////	if(bDoPower || bIsPower)
////////		HeightValue *= FMath::Pow(FBMValue, 2.0f); //this will give us terrain which consists mostly of flater land broken up occasionally by hills and valleys
////////	if(bDoBillowy)
////////		HeightValue *= FMath::Abs(FBMValue); //this will add more rolling hills
////////	if (bIsBillowy)
////////		HeightValue = FMath::Abs(HeightValue);
////////	if(bDoRigid)
////////		HeightValue *= 1 - FMath::Abs(FBMValue); //this will add sharp peaks or ridges as a possibility to occur
////////	if(bIsRigid)
////////		HeightValue = 1 - FMath::Abs(HeightValue);
////////
////////	if(bDoIsland)
////////		HeightValue -= SquareGradient(XPosition, YPosition); 	//determine how much the height will decrease based on the sqaure gradient map
////////	if(bDoTerrace)
////////		HeightValue = FMath::RoundFromZero(HeightValue * TerraceSize) / TerraceSize;//terrace the terrain by rouding each points height to its nearest multiple of TerraceSize
////////	//UE_LOG(LogTemp, Warning, TEXT("Height: %f"), HeightValue)
////////
////////	HeightValue *= PerlinScale; //give the Z position its final in game height
////////
////////	return HeightValue;
////////}
////////
////////
////////float AProcedurallyGeneratedTerrain::SquareGradient(float XPosition, float YPosition) //determine a square gradient to reduce the border of the map by
////////{
////////	//determine the value of the vertex's X and Y positions between -1 and 0
////////	float X = XPosition / Width * 2 - 1;
////////	float Y = YPosition / Height * 2 - 1;
////////
////////	float Value = FMath::Max(FMath::Abs(X), FMath::Abs(Y)); //for a sqaure gradient determine the positive value closest to the edge
////////
////////	//using this specific S curve equation compute the amount to reduce the terrains height by
////////	Value = FMath::Pow(Value, Size) / (FMath::Pow(Value, Size) + FMath::Pow((Steepness - Steepness * Value), Size)) - AboveWater;
////////	
////////	return Value;
////////}

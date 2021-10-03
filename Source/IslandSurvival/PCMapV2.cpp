// Fill out your copyright notice in the Description page of Project Settings.


#include "PCMapV2.h"
#include "ProceduralMapLighting.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APCMapV2::APCMapV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //"name" this is the name it will appear as in inspector
	Biomes = CreateDefaultSubobject<UBiomesComponent>("Biomed Component"); //"name" this is the name it will appear as in inspector

	BiomeGeneration = CreateDefaultSubobject<UBiomeGenerationComponent>("Biome Generation Component"); //"name" this is the name it will appear as in inspector
	BiomeGeneration->TerrainGenerator = this;

	Width = 100;
	Height = 100;
	GridSize = 100;

	Octaves = 4;
	Lacunarity = 2.5f;
	Grain = 0.5f;

	PerlinScale = 5000;
	PerlinRoughness = 0.01f;

	DomainAmount = 4.0f;

	Size = 5.0f;
	Steepness = 2.0f;
}

// Called when the game starts or when spawned
void APCMapV2::BeginPlay()
{
	Super::BeginPlay();

	//make a new map when generating in the world
	ClearMap();
	BiomeGeneration->BiomeAtEachPoint.Init(0, Width * Height); //at the beginning initilize each point to be ocean
	Normals.Init(FVector::ZeroVector, Width * Height);
	VerticeColours.Init(FLinearColor(1, 1, 1), Width * Height);
	GenerateSeed();
	CreateMesh();
	bRegenerateMap = false;
}
bool APCMapV2::ShouldTickIfViewportsOnly() const //run the code within the viewport when not running
{
	return true;
}
// Called every frame
void APCMapV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	////UE_LOG(LogTemp, Error, TEXT("Terrain Gen Actual Runtime Stats are: %i"), VerticeColours.Num())
	if (bRegenerateMap)
	{
		ClearMap();
		BiomeGeneration->BiomeAtEachPoint.Init(1, Width * Height); //at the beginning initilize each point to be ocean
		Normals.Init(FVector::ZeroVector, Width * Height);
		VerticeColours.Init(FLinearColor(1, 1, 1), Width * Height);
		GenerateSeed();
		CreateMesh();
		bRegenerateMap = false;
	}
}

void APCMapV2::ClearMap() //empties the map removing all data for it
{
	Vertices.Empty();
	VerticeColours.Empty();
	IslandNumber.Empty();

	Triangles.Empty();
	UVCoords.Empty();

	Normals.Empty();
	Tangents.Empty();


	OcataveOffsets.Empty();
	//TriangleNormals.Empty();
	//for the generation of the biomes empty all arrays, as it is reset
	BiomeGeneration->IslandKeys = 0;
	BiomeGeneration->IslandPointsMap.Empty();
	BiomeGeneration->BiomeAtEachPoint.Empty();

	MeshComponent->ClearAllMeshSections(); //removes all mesh sections, returning it to empty state
}

void APCMapV2::CreateMesh() //make the map generate populating all the nessesary data
{
	int32 TriangleCalculating = 0; //the current triangle of the mesh which is being calculated
	for (int32 i = 0; i < Height; i++)
	{
		for (int32 j = 0; j < Width; j++)
		{
			float ZPosition = GenerateHeight(j, i); //get the specific height for the point of the mesh

			Vertices.Add(FVector(i * GridSize, j * GridSize, ZPosition));
			////////////////////////////////if (ZPosition / PerlinScale > 0.05f)
			////////////////////////////////	VerticeColours.Add(FLinearColor(1, 1, 1)); //assign the colour of each vertex based on its Z position
			////////////////////////////////else if(ZPosition / PerlinScale > 0.0f && ZPosition / PerlinScale < 0.25f)
			////////////////////////////////	VerticeColours.Add(FLinearColor(0, 0.7f, 0)); //assign the colour of each vertex based on its Z position
			////////////////////////////////else
			////////////////////////////////	VerticeColours.Add(FLinearColor(1, 0.9f, 0)); //assign the colour of each vertex based on its Z position

			/*
				To get the position of an element in the array use i * width + j as its 1D but we are using 2D co-ordinates

				Generate the 2 triangles for the square grid point at once
				Triangle 1 (i, j), (i, j + 1), (i + 1, j)
				Triangle 2 (i + 1, j), (i, j + 1), (i + 1, j + 1)

				only generate a triangle if i+1 and j+1 will be values within the bounds of the Vertex array
			*/
			if (i + 1 < Height && j + 1 < Width) //this works as once loops finished will have the specific points using added to the array
			{
				Triangles.Add(i * Width + (j + 1)); Triangles.Add((i + 1) * Width + j); Triangles.Add(i * Width + j);
				Triangles.Add((i + 1) * Width + (j + 1)); Triangles.Add((i + 1) * Width + j); Triangles.Add(i * Width + (j + 1));
			}
			if (j > 0 && i > 0) //for each triangle of the mesh determine its normal using the cross product method
			{
				//get the vertex of the first triangle
				//then get vertex of second triangle
				//6 points shift each time through to get the next triangle in the array
				for (int32 k = 0; k < 2; k++)
				{

					/// <summary>
					/// error here as going down, not accross
					/// </summary>
///////////////////////					UE_LOG(LogTemp, Warning, TEXT("Vertices Count: %i, %i, %i, %i, %i"), i, j, Triangles.Num(), Vertices.Num(), TriangleCalculating)
						int32 trianlgeAtInArray = TriangleCalculating * 3;

					FVector A = Vertices[Triangles[trianlgeAtInArray]];
					FVector B = Vertices[Triangles[trianlgeAtInArray + 1]];
					FVector C = Vertices[Triangles[trianlgeAtInArray + 2]];

					FVector AB = A-B;
					FVector AC = A-C;
					FVector TrianlgeNormal = FVector::CrossProduct(AB, AC);
					TrianlgeNormal.Normalize(0.0001f); //this will get us just a vector pointing in a specific direction
					//TriangleNormals.Add(TrianlgeNormal);

					Normals[Triangles[trianlgeAtInArray]] += TrianlgeNormal;
					Normals[Triangles[trianlgeAtInArray + 1]] += TrianlgeNormal;
					Normals[Triangles[trianlgeAtInArray + 2]] += TrianlgeNormal;



					TriangleCalculating++;
				}
			}

			//as now have the normal for each triangle can calculate normal for each vertex

			UVCoords.Add(FVector2D(i, j));
		}
	}
	for(int32 x = 0; x < Normals.Num(); x++)
	{
		FVector item = Normals[x];
		item.Normalize(0.0001f);
		Normals[x] = -item;		

	}
	//determine the biome of each vertex of the map which is above water
	BiomeGeneration->VerticesBiomes();
	//spawn in all the appropriate meshes

	////UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, NormalsEmptyToNotUse, Tangents); //auto generate the normals and tangents for mesh and add them to respective array
	MeshComponent->CreateMeshSection_LinearColor(int32(0), Vertices, Triangles, Normals, UVCoords, VerticeColours, Tangents, true);
	UE_LOG(LogTemp, Warning, TEXT("Vertices Count: %i, Normals: %i, Biomes Count: %i, Islands Count: %i"), Vertices.Num(), Normals.Num(), BiomeGeneration->BiomeAtEachPoint.Num(), BiomeGeneration->IslandPointsMap.Num())
		/*for (int32 x = 0; x < NormalsEmptyToNotUse.Num(); x++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Normal x: %s"), *Normals[x].ToString())
			UE_LOG(LogTemp, Warning, TEXT("Normals Not Use Ever x: %s"), *NormalsEmptyToNotUse[x].ToString())

		}*/
}

float APCMapV2::FractalBrownianMotion(int32 XPosition, int32 YPosition)
{
	float HeightSum = 0; //the sum of the height at each octave
	float Frequency = 1; //*lacunarity
	float Amplitude = 1; //*grain

	FVector2D DSum = FVector2D(0, 0);

	for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
	{
		float NoiseValue = FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffsets[i], YPosition + OcataveOffsets[i]) * Frequency * PerlinRoughness); //the noise value for the octave
	
		DSum += FVector2D(0.15f, 0.15f);
		HeightSum += NoiseValue * Amplitude;// / (1 + FVector2D::DotProduct(DSum, DSum));

		Frequency *= Lacunarity;
		Amplitude *= Grain; //persistance(influence of amplitude on each sucessive octave
	}
	return HeightSum;
}

void APCMapV2::GenerateSeed() //give a random seed, otherwise use the specified one from editor
{
	if (bRandomSeed) //14625
	{
		Stream.GenerateNewSeed(); //this generates us a new random seed
		Seed = Stream.GetCurrentSeed();
	}
	else
	{
		Stream.Initialize(Seed); //initilizes the RNG with a specific seed
	}
	for (int32 i = 0; i < Octaves; i++) //for each octave use a different offset value for the noise 
	{
		float OffsetValue = Stream.RandRange(-10000.0f, 10000.0f); //offset so the noise will always produce a different random map
		OcataveOffsets.Add(OffsetValue);
	}
	//any varlues which are randomly generated for the biomes go here
}


float APCMapV2::DomainWarping(float XPosition, float YPosition)
{
	FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
	FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));

	float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);

	return NewHeight;
}


float APCMapV2::GenerateHeight(int32 XPosition, int32 YPosition) //all the functions for determining the height of a specific point
{
	float FBMValue;
	if (bDoDomain)
		FBMValue = DomainWarping(XPosition, YPosition);
	else
		FBMValue = FractalBrownianMotion(XPosition, YPosition);

	float HeightValue = FBMValue * FMath::Pow(FBMValue, 2.0f);
	////UE_LOG(LogTemp, Error, TEXT("Current Actual ZHeight is: %f"), HeightValue)

	HeightValue *= FMath::Abs(FBMValue); //this will give us more isolated mountain peaks and valleys
	HeightValue *= 1 - FMath::Abs(FBMValue);
	//UE_LOG(LogTemp, Warning, TEXT("Clampped HeightValue ZHeight is: %f"), HeightValue)

	//https://paginas.fe.up.pt/~ei12054/presentation/documents/thesis.pdf pg 39

	///////HeightValue = FMath::Clamp(HeightValue, 0.0f, 1.0f);
	if(bDoFalloff)
		HeightValue -= SquareGradient(XPosition, YPosition);


	if(bDoTerracing)
		HeightValue = FMath::RoundFromZero(HeightValue * TerraceSize) / TerraceSize;

	////UE_LOG(LogTemp, Warning, TEXT("Height of Point is: %f, %i, %i, %i, %i"), HeightValue, Vertices.Num(), IslandNumber.Num(), YPosition, XPosition)
	HeightValue = BiomeGeneration->AddIslandPoint(XPosition, YPosition, HeightValue); //for each point determine the island it specifically relates to
	//FLinearColor BiomeColour = FLinearColor(1, 1, 1);//Biomes->DetermineBiome(XPosition, YPosition, VerticeColours, Width, HeightValue);
	//VerticeColours.Add(BiomeColour);

	HeightValue *= PerlinScale;

	//////////////////HeightValue = FMath::Clamp(HeightValue, -PerlinScale / 5, PerlinScale);
	//HeightValue = FMath::Clamp(HeightValue, -100.0f, 1000000.0f);
	return HeightValue;
}


float APCMapV2::SquareGradient(float XPosition, float YPosition) //really need to fix it up
{
	FVector2D CentrePosition = FVector2D(Width / (2), Height / (2));
	float Dist = FVector2D::Distance(FVector2D(XPosition, YPosition), CentrePosition);
	Dist /= Width;
	//first need to get a point in the form of -1 to 0 otherwise will only do 2 of the edges of the map
	float X = (XPosition / Width) * 2 - 1;
	float Y = YPosition / Height * 2 - 1;

	float Value =FMath::Max(FMath::Abs(X), FMath::Abs(Y)); //* FMath::Sqrt(FMath::Pow(FMath::Abs(X), 4) + FMath::Pow(FMath::Abs(Y), 4)); //find the value which is closest to 1
	float newValue = 0;
	//if (Dist > Size) { FMath::Sqrt(FMath::Pow(X, 4) + FMath::Pow(Y, 4));
	newValue = FMath::Pow(Value, Size) / (FMath::Pow(Value, Size) + FMath::Pow((Steepness - Steepness * Value), Size)) - AboveWater;			//}
		//1 / FMath::Pow(Steepness - Steepness * Value, Size) - AboveWater;
		if (Value > .8f)
	{
		//Value = ////FMath::Sqrt(FMath::Pow(X, 4) + FMath::Pow(Y, 4));//////((Value - Size) / (1 - Size)) * (1 - 0) + 0;
		////////////////Value /= 10;
		/////////////UE_LOG(LogTemp, Warning, TEXT("New Value: %f"), Value)


			//newValue = FMath::RoundFromZero(newValue * 12) / 12;
			Value = ((Value - 0.8f) / (1 - 0.8f)) * (1 - 0) + 0; //min-max normalize the value
			return newValue;// / 10;///////newValue * FalloffScale;// / 2.5f - 0.1f;//((Dist - Size) / (1 - Size)) * (1 - 0) + 0 normalised value junk
	//other falloff equation (3*FMath::Pow(Dist, 2) - 2 * FMath::Pow(Dist, 3));
	}
	else
		return newValue;
}

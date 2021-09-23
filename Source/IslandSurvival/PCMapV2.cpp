// Fill out your copyright notice in the Description page of Project Settings.


#include "PCMapV2.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APCMapV2::APCMapV2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //"name" this is the name it will appear as in inspector
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
	
}
bool APCMapV2::ShouldTickIfViewportsOnly() const //run the code within the viewport when not running
{
	return true;
}
// Called every frame
void APCMapV2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMap)
	{
		ClearMap();
		GenerateSeed();
		CreateMesh();
		bRegenerateMap = false;
	}
}

void APCMapV2::ClearMap() //empties the map removing all data for it
{
	Vertices.Empty();
	VerticeColours.Empty();
	Triangles.Empty();
	UVCoords.Empty();

	OcataveOffsets.Empty();

	MeshComponent->ClearAllMeshSections(); //removes all mesh sections, returning it to empty state
}

void APCMapV2::CreateMesh() //make the map generate populating all the nessesary data
{
	for (int32 i = 0; i < Width; i++)
	{
		for (int32 j = 0; j < Height; j++)
		{
			float ZPosition = GenerateHeight(i, j); //get the specific height for the point of the mesh

			Vertices.Add(FVector(i * GridSize, j * GridSize, ZPosition));
			VerticeColours.Add(FLinearColor(ZPosition, ZPosition, ZPosition)); //assign the colour of each vertex based on its Z position

			/*
				To get the position of an element in the array use i * width + j as its 1D but we are using 2D co-ordinates

				Generate the 2 triangles for the square grid point at once
				Triangle 1 (i, j), (i, j + 1), (i + 1, j)
				Triangle 2 (i + 1, j), (i, j + 1), (i + 1, j + 1)

				only generate a triangle if i+1 and j+1 will be values within the bounds of the Vertex array
			*/
			if (i + 1 < Width && j + 1 < Height)
			{
				Triangles.Add(j * Width + (i + 1)); Triangles.Add((j + 1) * Width + i); Triangles.Add(j * Width + i);
				Triangles.Add((j + 1) * Width + (i + 1)); Triangles.Add((j + 1) * Width + i); Triangles.Add(j * Width + (i + 1));
			}

			UVCoords.Add(FVector2D(i, j));
		}
	}
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, Normals, Tangents); //auto generate the normals and tangents for mesh and add them to respective array
	MeshComponent->CreateMeshSection_LinearColor(int32(0), Vertices, Triangles, Normals, UVCoords, VerticeColours, Tangents, true);
	UE_LOG(LogTemp, Warning, TEXT("Vertices Count: %i, UVCoords Count: %i, Triangles Count: %i"), Vertices.Num(), UVCoords.Num(), Triangles.Num())
}

float APCMapV2::FractalBrownianMotion(int XPosition, int YPosition)
{
	float HeightSum = 0; //the sum of the height at each octave
	float Frequency = 1; //*lacunarity
	float Amplitude = 1; //*grain

	FVector2D DSum = FVector2D(0, 0);

	for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
	{
		float NoiseValue = FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffsets[i], YPosition + OcataveOffsets[i]) * Frequency * PerlinRoughness); //the noise value for the octave
		
		DSum += FVector2D(0.15f, 0.15f);
		HeightSum += NoiseValue * Amplitude / (1 + FVector2D::DotProduct(DSum, DSum));

		Frequency *= Lacunarity;
		Amplitude *= Grain; //persistance(influence of amplitude on each sucessive octave
	}
	return HeightSum;
}

void APCMapV2::GenerateSeed() //give a random seed, otherwise use the specified one from editor
{
	if (bRandomSeed)
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
}


float APCMapV2::DomainWarping(float XPosition, float YPosition)
{
	FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
	FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));

	float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);

	return NewHeight;
}



float APCMapV2::GenerateHeight(int XPosition, int YPosition) //all the functions for determining the height of a specific point
{
	float FBMValue;
	if (bDoDomain)
		FBMValue = DomainWarping(XPosition, YPosition);
	else
		FBMValue = FractalBrownianMotion(XPosition, YPosition);

	float HeightValue = FBMValue *FMath::Pow(FBMValue, 2.0f);

	HeightValue *= FMath::Abs(FBMValue); //this will give us more isolated mountain peaks and valleys
	HeightValue *= 1 - FMath::Abs(FBMValue);
	//https://paginas.fe.up.pt/~ei12054/presentation/documents/thesis.pdf pg 39

	HeightValue -= SquareGradient(XPosition, YPosition);

	HeightValue *= PerlinScale;
	//HeightValue = FMath::Clamp(HeightValue, -100.0f, 1000000.0f);
	return HeightValue;
}


float APCMapV2::SquareGradient(float XPosition, float YPosition)
{
	FVector2D CentrePosition = FVector2D(Width / (2), Height / (2));
	float Dist = FVector2D::Distance(FVector2D(XPosition, YPosition), CentrePosition);
	Dist /= Width;
	//first need to get a point in the form of -1 to 0 otherwise will only do 2 of the edges of the map
	float X = (XPosition / Width) * 2 - 1;
	float Y = YPosition / Height * 2 - 1;

	float Value = FMath::Max(FMath::Abs(X), FMath::Abs(Y)); //find the value which is closest to 1
	float newValue = 0;
	//if (Dist > Size) {
		newValue = FMath::Pow(Value, Size) / (FMath::Pow(Value, Size) + FMath::Pow((Steepness - Steepness * Value), Size));
		UE_LOG(LogTemp, Warning, TEXT("New Value: %f"), newValue)
			//}
		return newValue / 10;//((Dist - Size) / (1 - Size)) * (1 - 0) + 0 normalised value junk
		//other falloff equation (3*FMath::Pow(Dist, 2) - 2 * FMath::Pow(Dist, 3));
}

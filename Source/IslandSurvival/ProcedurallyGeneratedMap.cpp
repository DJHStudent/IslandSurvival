// Fill out your copyright notice in the Description page of Project Settings.



#include "ProcedurallyGeneratedMap.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AProcedurallyGeneratedMap::AProcedurallyGeneratedMap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //"name" this is the name it will appear as in inspector

	PerlinScale = 1000.0f;
	PerlinRoughness = 0.1f;

	Grain = 0.5;
	Lacunarity = 2;
	Octaves = 3;

	bRegenerateMap = false;
}

// Called when the game starts or when spawned
void AProcedurallyGeneratedMap::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProcedurallyGeneratedMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bRegenerateMap)
	{
		ClearMap();
		GenerateMap();
		bRegenerateMap = false;
	}
}

void AProcedurallyGeneratedMap::GenerateMap() //make the map generate populating all the nessesary data
{
	float PerlinOffset = FMath::RandRange(-10000.0f, 10000.0f); //offset so the noise will always produce a different random map
	OcataveOffset.Empty();
	for (int32 i = 0; i < Octaves; i++) //for each octave use a different noise value offset
	{
		float PerlinOffsets = FMath::RandRange(-10000.0f, 10000.0f); //offset so the noise will always produce a different random map
		OcataveOffset.Add(PerlinOffsets);
	}
	for (int32 i = 0; i < Width; i++)
	{
		for (int32 j = 0; j < Height; j++)
		{
			float ZPosition = CalculateHeight(i, j, PerlinOffset); //use their position on grid, not their real world values
			Vertices.Add(FVector(i * GridSize, j * GridSize, ZPosition));
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
	MeshComponent->CreateMeshSection(int32(0), Vertices, Triangles, Normals, UVCoords, TArray<FColor>(), Tangents, true);
	UE_LOG(LogTemp, Warning, TEXT("Vertices Count: %i, UVCoords Count: %i, Triangles Count: %i"), Vertices.Num(), UVCoords.Num(), Triangles.Num())
}

void AProcedurallyGeneratedMap::ClearMap() //empties the map removing all data for it
{
	Vertices.Empty();
	Triangles.Empty();
	UVCoords.Empty();

	MeshComponent->ClearAllMeshSections(); //removes all mesh sections, returning it to empty state
}

bool AProcedurallyGeneratedMap::ShouldTickIfViewportsOnly() const
{
	return true;
}

float AProcedurallyGeneratedMap::CalculateHeight(float XPosition, float YPosition, float PerlinOffset)
{ //includes frequency, amplitude, octaves, lacunarity, grain
	float ZHeight = 0;

	float Frequency = 1; //*lacunarity
	float Amplitude = 1; //*grain
	FVector2D Centre = FVector2D(Width / 2, Height / 2);
	float DistFromCentre = FVector2D::Distance(Centre, FVector2D(XPosition, YPosition));
	DistFromCentre /= Width;
	//DistFromCentre /= Octaves;
	//DistFromCentre *= Octaves;
	//UE_LOG(LogTemp, Warning, TEXT("Distance from centre: %f"), DistFromCentre)
	//float OctaveOffset = FMath::RandRange(-10000.0f, 10000.0f); //ensures that each Octave will be different as from differing point
	//if (DistFromCentre < .4f) {
		for (int32 i = 0; i < Octaves; i++)
		{
			//new height value
			ZHeight += (FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffset[i], YPosition + OcataveOffset[i]) * Frequency * PerlinRoughness)- DistFromCentre) * Amplitude * PerlinScale;
			//ZHeight *= Amplitude;

			Frequency *= Lacunarity;
			Amplitude *= Grain;
		}
		return 1 - FMath::Abs(ZHeight);
	//return -PerlinScale;
}

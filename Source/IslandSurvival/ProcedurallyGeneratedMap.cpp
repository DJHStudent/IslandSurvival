// Fill out your copyright notice in the Description page of Project Settings.



#include "ProcedurallyGeneratedMap.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
AProcedurallyGeneratedMap::AProcedurallyGeneratedMap()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("Mesh Component"); //"name" this is the name it will appear as in inspector
	////MeshComponent->SetCollisionEnabled();

	PerlinScale = 1000.0f;
	PerlinRoughness = 0.1f;

	Grain = 0.5;
	Lacunarity = 2;
	Octaves = 3;

	DomainAmount = 4.0f;
	bDoDomain = false;
	bRegenerateMap = false;

	derivativeSmoothing = 0.5f;
	TerraceSize = 12;

	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialOb(TEXT("Grass'/Game/Materials.Grass'"));
	//Material = MaterialOb.Object;
	//Material = .Object;
	//static ConstructorHelpers::FObjectFinder<UMaterial> MaterialOb(TEXT("Material'/Game/VertexMat.VertexMat'"));
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
			float ZPosition;
			if (bDoDomain)
				ZPosition = DomainWarping(i, j) * FMath::Pow(DomainWarping(i, j), 2); //use their position on grid, not their real world values
			else
			{
				ZPosition = CalculateHeight(i, j, 0) * PerlinScale;// *FMath::Pow(CalculateHeight(i, j, 0), 2);
				/*ZPosition -= SquareGradient(i, j, 0);
				ZPosition *= PerlinScale;*/
			}
			ZPosition -= SquareGradient(i, j, 0);
			//ZPosition = FMath::Clamp(ZPosition, -0.0f, 10000.0f);
			ZPosition *= PerlinScale;
			//https://paginas.fe.up.pt/~ei12054/presentation/documents/thesis.pdf pg 39

			Vertices.Add(FVector(i * GridSize, j * GridSize, ZPosition));
			VerticeColours.Add(FLinearColor((ZPosition / PerlinScale + 1) / 2, (ZPosition / PerlinScale + 1) / 2, (ZPosition / PerlinScale + 1) / 2));

			//////////////////////UE_LOG(LogTemp, Warning, TEXT("Current Z psotion: %f"), (ZPosition/ PerlinScale))
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
	//////FVector::Nor
//UKismetProceduralMeshLibrary::Normal
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVCoords, Normals, Tangents); //auto generate the normals and tangents for mesh and add them to respective array
	MeshComponent->CreateMeshSection_LinearColor(int32(0), Vertices, Triangles, Normals, UVCoords, VerticeColours, Tangents, true);
	//MeshComponent->SetMaterial(0, Material);
	UE_LOG(LogTemp, Warning, TEXT("Vertices Count: %i, UVCoords Count: %i, Triangles Count: %i"), Vertices.Num(), UVCoords.Num(), Triangles.Num())
}

void AProcedurallyGeneratedMap::ClearMap() //empties the map removing all data for it
{
	Vertices.Empty();
	VerticeColours.Empty();
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
	float radius = 0.45f;


	float ZHeight = 0;
	FVector2D DSum = FVector2D(0, 0);
	float Frequency = 1; //*lacunarity
	float Amplitude = 1; //*grain
	float CurrentGrain = Grain;

	FVector2D Centre = FVector2D(Width / 2, Height / 2);
	float DistFromCentre = FVector2D::Distance(Centre, FVector2D(XPosition, YPosition));
	DistFromCentre /= Width;
	//if (DistFromCentre < radius) //Shouldnt be a straight line but a curve which is flatter towards centre and after point goes quickly down
		//DistFromCentre = 0;
	////////////////////////////if (DistFromCentre < radius)
	////////////////////////////	DistFromCentre = 0;
	////////////////////////////else //convert all values into range of zero to 1
	////////////////////////////{
	////////////////////////////	DistFromCentre -= radius;
	////////////////////////////}
	//if (DistFromCentre < 0.6f)
		//DistFromCentre = 0;
	//DistFromCentre /= Octaves;
	//DistFromCentre *= Octaves;
	//UE_LOG(LogTemp, Warning, TEXT("Distance from centre: %f"), DistFromCentre)
	//float OctaveOffset = FMath::RandRange(-10000.0f, 10000.0f); //ensures that each Octave will be different as from differing point
	//if (DistFromCentre < .4f) {
	for (int32 i = 0; i < Octaves; i++)
	{
		//new height value
		float Value = FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffset[i], YPosition + OcataveOffset[i]) * Frequency * PerlinRoughness);
		//Value = FMath::Abs(Value);

		DSum += FVector2D(derivativeSmoothing, derivativeSmoothing);


		ZHeight += (Value * Amplitude) / (1 + FVector2D::DotProduct(DSum, DSum));
		//ZHeight *= Amplitude;


		//ZHeight /= 1 + FVector2D::DotProduct(DSum, DSum);

		Frequency *= Lacunarity;
		Amplitude *= CurrentGrain;

		CurrentGrain += GrainAmplification;
	}
	//if (ZHeight > 1)
		//UE_LOG(LogTemp, Error, TEXT("%f, %f"), ZHeight, DistFromCentre)
	//ZHeight += 1 - FMath::Abs(ZHeight);



    //// ZHeight = 1 - FMath::Abs(ZHeight);

	//ZHeight = FMath::RoundFromZero(ZHeight * TerraceSize) / TerraceSize;//FMath::Pow(FMath::Sin((ZHeight - FMath::RoundFromZero(ZHeight)) * 2.45f), 2) + FMath::RoundFromZero(ZHeight);//  //the wau to get terraced terrain if thats what we want


	//ZHeight = FMath::Clamp(ZHeight, 0.0f, 1.0f);//DistFromCentre;  //SquareGradient(XPosition, YPosition, DistFromCentre);
	///////ZHeight -= SquareGradient(XPosition, YPosition, 0);
	return ZHeight;//doing sharp peaks(1 - FMath::Abs(ZHeight)) * PerlinScale;
//return -PerlinScale;
}

float AProcedurallyGeneratedMap::SquareGradient(float XPos, float YPos, float CentreDist)
{
	//first need to get a point in the form of -1 to 0 otherwise will only do 2 of the edges of the map
	float X = XPos / Width * 2 - 1;
	float Y = YPos / Height * 2 - 1;

	float Value = FMath::Max(FMath::Abs(X), FMath::Abs(Y)); //find the value which is closest to 1




	float a = 8.0f;
	float b = 8;

	
	float newValue = FMath::Pow(Value, Size) / (FMath::Pow(Value, Size) + FMath::Pow((Steepness - Steepness * Value), Size));//Steepness * FMath::Pow(Value, 2);//1 / (1 + FMath::Exp(-Steepness * Value));//FMath::Pow(Value, Steepness) / (FMath::Pow(Value, Steepness) + FMath::Pow(Size - Size * Value, Steepness));
	//1 / (1 + FMath::Exp(-Steepness * Value));//Steepness * FMath::Pow(Value, 2);////////
	return newValue;
	//float XDistZero = XPos; float YDistZero = YPos;
	//float XDistWidth = Width - XPos; float YDistHeight = Height - YPos;

	//float value = FMath::Max(FMath::Abs(XPos / Width * 2 - 1), FMath::Abs(YPos / Width * 2 - 1));

	//float a = 3;
	//float b = 2.2f;
	//float islandGradientValue = value;//FMath::Pow(value, a) / (FMath::Pow(value, a) + FMath::Pow(b - b * value, a));

	//return islandGradientValue;
	////if (CentreDist > 0.45f) 
	//{
	//	//close to X = 0
	//	if (XDistZero <= XDistWidth && XDistZero <= YDistHeight && XDistZero <= YDistZero)
	//		return 1 - XDistZero / Width;

	//	else if (XDistWidth <= XDistZero && XDistWidth <= YDistHeight && XDistWidth <= YDistZero)
	//		return XDistWidth / Width;

	//	else if (YDistZero <= YDistHeight && YDistZero <= XDistWidth && YDistZero <= XDistZero)
	//		return 1 - YDistZero / Height;

	//	else if (YDistHeight <= YDistZero && YDistHeight <= XDistWidth && YDistHeight <= XDistZero)
	//		return YDistHeight / Height;
	//}
	//return 0.0f;
}

float AProcedurallyGeneratedMap::DomainWarping(float XPos, float YPos)
{
	FVector2D q = FVector2D(CalculateHeight(XPos, YPos, 0), CalculateHeight(XPos + 5.2f, YPos + 1.3f, 0));

	FVector2D r = FVector2D(CalculateHeight(XPos + DomainAmount * q.X + 1.7f, YPos + DomainAmount * q.Y + 9.2f, 0), CalculateHeight(XPos + DomainAmount * q.X + 8.3f, YPos + DomainAmount * q.Y + 2.8f, 0));


	float NewHeight = CalculateHeight(XPos + DomainAmount * r.X, YPos + DomainAmount * r.Y, 0);

	/////////UE_LOG(LogTemp, Error, TEXT("Height: %f"), NewHeight)

		//if (NewHeight > 0.3f)
		{
			//NewHeight = FMath::Abs(NewHeight);
			//NewHeight *= 2500;
		}
	//	else
		//NewHeight = FMath::Clamp(NewHeight, 0.0f, 1.0f);
		//NewHeight -= SquareGradient(XPos, YPos, 0);
			//NewHeight *= PerlinScale;
	return NewHeight;
}
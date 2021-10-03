// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomesComponent.h"

// Sets default values for this component's properties
UBiomesComponent::UBiomesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	//TemperatureOffset = 0;
	//MoistureOffset = 0;
	DomainAmount = 80;
	// ...
}


// Called when the game starts
void UBiomesComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBiomesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UBiomesComponent::PointValue(int32 i, int32 j, const float& Offset)
{
	//int32 Octaves = 10;


	//float HeightSum = 0; //the sum of the height at each octave
	//float Frequency = 1; //*lacunarity
	//float Amplitude = 1; //*grain

	//for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
	//{
	//	float NoiseValue = FMath::PerlinNoise2D(FVector2D(i + Offset, j + Offset) * Frequency * NoiseJunky); //the noise value for the octave

	//	HeightSum += NoiseValue * Amplitude;

	//	Frequency *= 2;
	//	Amplitude *= 0.5f; //persistance(influence of amplitude on each sucessive octave
	//}
	//return HeightSum;
	return FMath::PerlinNoise2D(FVector2D(i + Offset, j + Offset) * NoiseJunky);
}

FLinearColor UBiomesComponent::DetermineBiome(int32 i, int32 j, TArray<FLinearColor>& VerticeColours, int32 Width, float Height)
{
	if (Height <= 0) //ie. the waterline do the stuff for generating the underwater biomes
	{
		for (int32 a = 0; a < HeightBasedBiomes.Num(); a++)
		{
			if (Height >= HeightBasedBiomes[a].MinHeight && Height <= HeightBasedBiomes[a].MaxHeight)
			{
				CurrCellBiome.Add(0); //ok this will need to be redone///////////////////////////////////////////////////////////
				return HeightBasedBiomes[a].Colour;
				break;
			} 
		}
	}
	else
	{
		float TempValue = DomainWarping(i, j, TemperatureOffset);
		float MoistureValue = DomainWarping(i, j, MoistureOffset);

		for (int32 a = 0; a < DifferentBiomes.Num(); a++)
		{
			if (TempValue >= DifferentBiomes[a].MinTemp && TempValue <= DifferentBiomes[a].MaxTemp)
			{
				if (MoistureValue >= DifferentBiomes[a].MinMoisture && MoistureValue <= DifferentBiomes[a].MaxMoisture)
				{
					//if the current height for the specified biome is within a range change the biome to the new one
					CurrCellBiome.Add(a);
					return DifferentBiomes[a].Colour; //BiomeBlend(i, j, a, VerticeColours, Width);
					break;
				}
			}
			//if between min and max temp &&&& between min and max moisture
			//return linear color at the specific point
			//break
		}
		//UE_LOG(LogTemp, Error, TEXT("Values for biomes which failed: %f, %f, %f"), TempValue, MoistureValue)
	}
	return FLinearColor(0, 0, 0); //should never actually be needed but just in case return null
}

void UBiomesComponent::CheckOffset()
{
	CurrCellBiome.Empty();
}


float UBiomesComponent::DomainWarping(float XPosition, float YPosition, const float& Offset)
{
	FVector2D q = FVector2D(PointValue(XPosition, YPosition, Offset), PointValue(XPosition + 5.2f, YPosition + 1.3f, Offset));
	FVector2D r = FVector2D(PointValue(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f, Offset), PointValue(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f, Offset));

	float NewHeight = PointValue(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y, Offset);
	
	return NewHeight;
}

FLinearColor UBiomesComponent::BiomeBlend(int32 i, int32 j, int32 CurrBiome, TArray<FLinearColor>& VerticeColours, int32 Width)
{
	FLinearColor Color = DifferentBiomes[CurrBiome].Colour;
	if (j - 1 >= 0)
	{
		 //get a node one behind the current node, if exists
		if (CurrCellBiome[(i)*Width + (j - 1)] != CurrBiome)
		{
			Color = FMath::Lerp(Color, DifferentBiomes[CurrCellBiome[(i)*Width + (j - 1)]].Colour, Alpha);
			//FLinearColor OtherBiomeColour = VerticeColours[(i)*Width + (j - 1)];
			//VerticeColours[(i)*Width + (j - 1)] = FMath::Lerp(OtherBiomeColour, DifferentBiomes[CurrBiome].Colour, Alpha);
		}
	}
	//get node one up and back is j - i, i - 1
	if (j - 1 >= 0 && i - 1 >= 0)
	{
		//get a node one diagonally up and back from the current node, if exists
		if (CurrCellBiome[(i - 1) * Width + (j - 1)] != CurrBiome)
		{
			Color = FMath::Lerp(Color, DifferentBiomes[CurrCellBiome[(i - 1) * Width + (j - 1)]].Colour, Alpha);
			//FLinearColor OtherBiomeColour = VerticeColours[(i - 1)*Width + (j - 1)];
			//VerticeColours[(i - 1)*Width + (j - 1)] = FMath::Lerp(OtherBiomeColour, DifferentBiomes[CurrBiome].Colour, Alpha);
		}
	}
	//get node one up is i - 1
	if (i - 1 >= 0)
	{
		//get a node one up from the current node, if exists
		if (CurrCellBiome[(i - 1) * Width + (j)] != CurrBiome)
		{
			Color = FMath::Lerp(Color, DifferentBiomes[CurrCellBiome[(i - 1) * Width + (j)]].Colour, Alpha);
			//FLinearColor OtherBiomeColour = VerticeColours[(i - 1)*Width + (j)];
			//VerticeColours[(i - 1)*Width + (j)] = FMath::Lerp(OtherBiomeColour, DifferentBiomes[CurrBiome].Colour, Alpha);
		}
	}
	//get node one up and forward is j + i, i - 1
	if (j + 1 < Width && i - 1 >= 0)
	{
		 //get a node one diagonally up and forward of the current node, if exists
		if (CurrCellBiome[(i - 1) * Width + (j + 1)] != CurrBiome) 
		{
			Color = FMath::Lerp(Color, DifferentBiomes[CurrCellBiome[(i - 1) * Width + (j + 1)]].Colour, Alpha);
			//FLinearColor OtherBiomeColour = VerticeColours[(i - 1) * Width + (j + 1)];
			//VerticeColours[(i - 1) * Width + (j + 1)] = FMath::Lerp(OtherBiomeColour, DifferentBiomes[CurrBiome].Colour, Alpha);
		}
	}
	return Color;
}
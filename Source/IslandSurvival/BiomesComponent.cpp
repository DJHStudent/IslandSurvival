// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomesComponent.h"

// Sets default values for this component's properties
UBiomesComponent::UBiomesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	TemperatureOffset = 0;
	MoistureOffset = 0;
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

float UBiomesComponent::PointValue(int32 i, int32 j, const int32& Offset)
{
	return FMath::PerlinNoise2D(FVector2D(i + Offset, j + Offset) * 0.008f);
}

FLinearColor UBiomesComponent::DetermineBiome(int32 i, int32 j)
{
	float TempValue = DomainWarping(i, j, TemperatureOffset);
	float MoistureValue = DomainWarping(i, j, MoistureOffset);
	

	for (int32 a = 0; a < DifferentBiomes.Num(); a++)
	{
		if (TempValue >= DifferentBiomes[a].MinTemp && TempValue <= DifferentBiomes[a].MaxTemp)
		{
			if (MoistureValue >= DifferentBiomes[a].MinMoisture && MoistureValue <= DifferentBiomes[a].MaxMoisture)
			{
				return DifferentBiomes[a].Colour;
				break;
			}
		}
		//if between min and max temp &&&& between min and max moisture
		//return linear color at the specific point
		//break
	}
	UE_LOG(LogTemp, Error, TEXT("Values for biomes which failed: %f, %f, %f"), TempValue, MoistureValue)
	return FLinearColor(0, 0, 0); //should never actually be needed but just in case return null
}


float UBiomesComponent::DomainWarping(float XPosition, float YPosition, float Offset)
{
	FVector2D q = FVector2D(PointValue(XPosition, YPosition, Offset), PointValue(XPosition + 5.2f, YPosition + 1.3f, Offset));
	FVector2D r = FVector2D(PointValue(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f, Offset), PointValue(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f, Offset));

	float NewHeight = PointValue(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y, Offset);

	return NewHeight;
}


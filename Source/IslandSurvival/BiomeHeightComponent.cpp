// Fill out your copyright notice in the Description page of Project Settings.


#include "BiomeHeightComponent.h"

// Sets default values for this component's properties
UBiomeHeightComponent::UBiomeHeightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBiomeHeightComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBiomeHeightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UBiomeHeightComponent::FractalBrownianMotion(int32 XPosition, int32 YPosition)
{
	float HeightSum = 0; //the sum of the height at each octave
	float Frequency = 1; //offset value for steepness of each successive octave
	float Amplitude = 1; //the offset value for the height at each octave

	for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
	{
		//for each octave determine the noise value to use, using frequency
		float NoiseValue = FMath::PerlinNoise2D(FVector2D(XPosition + OcataveOffsets[i], YPosition + OcataveOffsets[i]) * Frequency * PerlinRoughness);

		HeightSum += NoiseValue * Amplitude; //add this noise value to the total with amplitude

		Frequency *= Lacunarity; //the influence of the frequency over each sucessive octave, increasing
		Amplitude *= Grain; //influence of amplitude on each sucessive octave, decreasing
	}
	return HeightSum; //return the final height sum
}


float UBiomeHeightComponent::DomainWarping(float XPosition, float YPosition) //for each vertex offset its height by a specific amount of values, through combining multiple FBM noise
{
	//calculate the firest points X and Y position
	FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
	//determine the next points X and Y position based on q's point values
	FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));

	//Determine the final new height to give the point
	float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);

	return NewHeight;
}


float UBiomeHeightComponent::GenerateHeight(int32 XPosition, int32 YPosition) //all the functions for determining the height of a specific point
{
	float FBMValue = DomainWarping(XPosition, YPosition); //determine the inital value of the point using domain warping

	float HeightValue = FBMValue;
	if (bDoPower || bIsPower)
		HeightValue *= FMath::Pow(FBMValue, 2.0f); //this will give us terrain which consists mostly of flater land broken up occasionally by hills and valleys
	if (bDoBillowy)
		HeightValue *= FMath::Abs(FBMValue); //this will add more rolling hills
	if (bIsBillowy)
		HeightValue = FMath::Abs(HeightValue);
	if (bDoRigid)
		HeightValue *= 1 - FMath::Abs(FBMValue); //this will add sharp peaks or ridges as a possibility to occur
	if (bIsRigid)
		HeightValue = 1 - FMath::Abs(HeightValue);

	if (bDoTerrace)
		HeightValue = FMath::RoundFromZero(HeightValue * TerraceSize) / TerraceSize;//terrace the terrain by rouding each points height to its nearest multiple of TerraceSize

	HeightValue *= PerlinScale; //give the Z position its final in game height

	return HeightValue;
}
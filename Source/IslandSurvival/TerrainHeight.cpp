// Fill out your copyright notice in the Description page of Project Settings.


#include "TerrainHeight.h"

void UTerrainHeight::DeclareOffsetValues(FRandomStream Stream)
{
	OctaveOffsets.Empty(); //as resetting it will need to empty it
	for (int32 i = 0; i < Octaves; i++) //for each octave use a different offset value for the noise 
	{
		float OffsetValue = Stream.FRandRange(-10000.0f, 10000.0f); //offset each noise octave so it will always produce a different random map
		OctaveOffsets.Add(OffsetValue);
	}
}

float UTerrainHeight::FractalBrownianMotion(int32 XPosition, int32 YPosition)
{
	float HeightSum = 0; //the sum of the height at each octave
	float Frequency = 1; //offset value for steepness of each successive octave
	float Amplitude = 1; //the offset value for the height at each octave

	for (int32 i = 0; i < Octaves; i++) //the number of layers of noise to include
	{
		//for each octave determine the noise value to use, using frequency
		float NoiseValue = FMath::PerlinNoise2D(FVector2D(XPosition + OctaveOffsets[i], YPosition + OctaveOffsets[i]) * Frequency * PerlinRoughness);

		HeightSum += NoiseValue * Amplitude; //add this noise value to the total with amplitude

		Frequency *= Lacunarity; //the influence of the frequency over each sucessive octave, increasing
		Amplitude *= Grain; //influence of amplitude on each sucessive octave
	}
	return HeightSum; //return the final height sum
}


float UTerrainHeight::DomainWarping(const int32 XPosition, const int32 YPosition) //for each vertex offset its height by a specific amount of values, through combining multiple FBM noise
{
	//calculate the firest points X and Y position
	FVector2D q = FVector2D(FractalBrownianMotion(XPosition, YPosition), FractalBrownianMotion(XPosition + 5.2f, YPosition + 1.3f));
	//determine the next points X and Y position based on q's point values
	FVector2D r = FVector2D(FractalBrownianMotion(XPosition + DomainAmount * q.X + 1.7f, YPosition + DomainAmount * q.Y + 9.2f), FractalBrownianMotion(XPosition + DomainAmount * q.X + 8.3f, YPosition + DomainAmount * q.Y + 2.8f));

	//Determine the final new height to give the point
	float NewHeight = FractalBrownianMotion(XPosition + DomainAmount * r.X, YPosition + DomainAmount * r.Y);

	return NewHeight;
}

float UTerrainHeight::SquareGradient(const int32 XPosition, const int32 YPosition) //determine a square gradient to reduce the border of the map by
{
	//determine the value of the vertex's X and Y positions between -1 and 0
	float X = (float)XPosition / Width * 2 - 1;
	float Y = (float)YPosition / Height * 2 - 1;

	float Value = FMath::Max(FMath::Abs(X), FMath::Abs(Y)); //for a sqaure gradient determine the positive value closest to the edge

	//using this specific S curve equation compute the amount to reduce the terrains height by
	Value = FMath::Pow(Value, Steepness) / (FMath::Pow(Value, Steepness) + FMath::Pow((DistStarts -  Value), Steepness)) - AboveWater;

	return Value;
}

float UTerrainHeight::GenerateHeight(const int32 XPosition, const int32 YPosition, const float WaterZPos, const bool bSmooth) //all the functions for determining the height of a specific point
{
	float FBMValue; //determine the inital value of the point
	bDoWarping ? FBMValue = DomainWarping(XPosition, YPosition) : FBMValue = FractalBrownianMotion(XPosition, YPosition);
	
	float HeightValue = FBMValue;
	//try adding in the power value
	HeightValue = TerrainAdditionMode(PowerNoiseEnum, HeightValue, FMath::Pow(HeightValue, 2.0f));//this will give us terrain which consists mostly of flater land broken up occasionally by hills and valleys
	//try adding in the billowy value
	HeightValue = TerrainAdditionMode(BillowyNoiseEnum, HeightValue, FMath::Abs(HeightValue));
	//try adding in the rigid value
	HeightValue = TerrainAdditionMode(RigidNoiseEnum, HeightValue, 1 - FMath::Abs(HeightValue));


	if (bDoFalloff)
		HeightValue -= SquareGradient(XPosition, YPosition); 	//determine how much the height will decrease based on the sqaure gradient map
	if (bDoTerrace && !bSmooth)
		HeightValue = FMath::RoundFromZero(HeightValue * TerraceSize) / TerraceSize;//terrace the terrain by rouding each points height to its nearest multiple of TerraceSize

	switch (NoiseDepthEnum)
	{
		case ENoiseDepth::LandNoise: //only do noise between 0 and 1
			//minmax = (value - oldmin)/(oldmax-oldmin(always 2 here)) *(newmax - newmin) + newmin
			//add the offset so will always appear above the water, min = waterZpos max = 1
			HeightValue = ((HeightValue - -1) / 2) * (1 - WaterZPos) + WaterZPos;
			break;
		case ENoiseDepth::WaterNoise: //only do noise between -1 and 0
			//add offset so will always appear below water, min = -1 max = waterZpos
			HeightValue = ((HeightValue - -1) / 2) * (WaterZPos - -1) + -1;
			break;
		default: //do noise between the full -1 and 1
			//do nothing as result
			break;
	}

	HeightValue *= PerlinScale; //give the Z position its final in game height
	return HeightValue;
}

float UTerrainHeight::TerrainAdditionMode(ETerrainAdditions::Type AddMode, const float CurrentValue, const float AdditionalValue)
{
	switch (AddMode)
	{
		case ETerrainAdditions::FullUse:
			return AdditionalValue;
		case ETerrainAdditions::Add:
			return CurrentValue + AdditionalValue;
		case ETerrainAdditions::Subtract:
			return CurrentValue - AdditionalValue;
		case ETerrainAdditions::Multiply:
			return CurrentValue * AdditionalValue;
		case ETerrainAdditions::Divide:
			return CurrentValue / AdditionalValue;
		default: //don't use
			return CurrentValue;
	}
}
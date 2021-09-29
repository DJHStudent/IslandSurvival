// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BiomesComponent.generated.h"


USTRUCT()
struct FBiomeStatss //for the noise based biomes
{
    GENERATED_BODY()

        //~ The following member variable will be accessible by Blueprint Graphs:
        // This is the tooltip for our test variable.
    UPROPERTY(EditAnywhere)
    float MinTemp;
    UPROPERTY(EditAnywhere)
    float MaxTemp;

    UPROPERTY(EditAnywhere)
    float MinMoisture;
    UPROPERTY(EditAnywhere)
    float MaxMoisture;

	UPROPERTY(EditAnywhere)
	FLinearColor Colour;

	FBiomeStatss()
	{
		MinTemp = 0;
	MaxTemp = 0;
	MinMoisture = 0;
	MaxMoisture = 0;

	Colour = FLinearColor(0, 0, 0);
	}
};

USTRUCT()
struct FHeightBiomeStats
{
	GENERATED_BODY()

		//~ The following member variable will be accessible by Blueprint Graphs:
		// This is the tooltip for our test variable.
	UPROPERTY(EditAnywhere)
	float MinHeight;
	UPROPERTY(EditAnywhere)
	float MaxHeight;

	UPROPERTY(EditAnywhere)
		FLinearColor Colour;

	FHeightBiomeStats()
	{
		MinHeight = 0;
		MaxHeight = 0;

		Colour = FLinearColor(0, 0, 0);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDSURVIVAL_API UBiomesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBiomesComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float PointValue(int32 i, int32 j, const float& Offset);
	FLinearColor DetermineBiome(int32 i, int32 j, TArray<FLinearColor>& VerticeColours, int32 Width, float Height);

	float TemperatureOffset;
	float MoistureOffset;
	void CheckOffset();

private:

	TArray<int32> CurrCellBiome; //the current biome of each cell

	UPROPERTY(EditAnywhere, Category = "Noise Biomes")
    TArray<FBiomeStatss> DifferentBiomes;

	UPROPERTY(EditAnywhere, Category = "Height Biomes")
	TArray<FHeightBiomeStats> HeightBasedBiomes;


	UPROPERTY(EditAnywhere)
	float DomainAmount;
	float DomainWarping(float XPosition, float YPosition, const float& Offset);

	FLinearColor BiomeBlend(int32 i, int32 j, int32 CurrBiome, TArray<FLinearColor>& VerticeColours, int32 Width);
	UPROPERTY(EditAnywhere)
	float Alpha;

	UPROPERTY(EditAnywhere)
	float NoiseJunky;
};

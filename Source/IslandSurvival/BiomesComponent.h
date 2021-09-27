// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BiomesComponent.generated.h"


USTRUCT()
struct FBiomeStats
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

	FBiomeStats()
	{
		MinTemp = 0;
	MaxTemp = 0;
	MinMoisture = 0;
	MaxMoisture = 0;

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

	float PointValue(int32 i, int32 j, const int32& Offset);
	FLinearColor DetermineBiome(int32 i, int32 j);

	float TemperatureOffset;
	float MoistureOffset;
private:


	UPROPERTY(EditAnywhere)
	TArray<FBiomeStats> DifferentBiomes;

	float DomainAmount;
	float DomainWarping(float XPosition, float YPosition, float Offset);
};

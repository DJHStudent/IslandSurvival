// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BiomeHeightComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ISLANDSURVIVAL_API UBiomeHeightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBiomeHeightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GenerateHeight(int32 XPosition, int32 YPosition); //returns the height of each vertex in turn

	float TerraceSize;
	TArray<float> OcataveOffsets;
private:
	UPROPERTY(EditAnywhere)//the range of values perlin noise will return
		float PerlinScale;
	UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
		float PerlinRoughness;

	UPROPERTY(EditAnywhere, Category = "FBM")//number of perlin noise maps to layer
		int32 Octaves;
	UPROPERTY(EditAnywhere, Category = "FBM")//the gap in frequency from one layer to the next
		float Lacunarity;
	UPROPERTY(EditAnywhere, Category = "FBM")//how the amplitude changes over time
		float Grain;

	float FractalBrownianMotion(int32 XPosition, int32 YPosition);

	float DomainWarping(float XPos, float YPos); //offset the vertices of each point by specific values
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//the amount of offset to actually apply to each vertex
		float DomainAmount;

	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoTerrace;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoPower;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoRigid;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoBillowy;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoMeshes;
	UPROPERTY(EditAnywhere, Category = "Options") //the distance appart of each terrace, larger value means smaller appart
		bool bDoIsland;

	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsPower;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsRigid;
	UPROPERTY(EditAnywhere, Category = "IsOptions") //the distance appart of each terrace, larger value means smaller appart
		bool bIsBillowy;
		
};

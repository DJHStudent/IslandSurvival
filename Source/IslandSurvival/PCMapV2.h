// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BiomesComponent.h"
#include "BiomeGenerationComponent.h"
#include "PCMapV2.generated.h"

UCLASS()
class ISLANDSURVIVAL_API APCMapV2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APCMapV2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)//number of grid squares in X
	int32 Width; 
	UPROPERTY(EditAnywhere)//number of grid squares in Y
	int32 Height; 
	UPROPERTY(EditAnywhere)//size of each grid square
	float GridSize; 

	TArray<FVector> Vertices; //the position of each point in world space
	TArray<FLinearColor> VerticeColours; //the colour of each point
	TArray<int32> IslandNumber; //list directly correlating to the island each vertice relates to

	TArray<int32> Triangles;
	TArray<FVector2D> UVCoords; //coordinates to specify how a texture will be applied

		//texturing arrays
	TArray<FVector> Normals;
	TArray<FVector> NormalsEmptyToNotUse;
	TArray<FProcMeshTangent> Tangents;

	UFUNCTION(BlueprintCallable)
	void CreateMesh();
	void ClearMap();

	UPROPERTY(EditAnywhere)//the range of values perlin noise will return
	float PerlinScale; 
	UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
	float PerlinRoughness; 


	virtual bool ShouldTickIfViewportsOnly() const override; //update the maps terrain, when value changed even if in the editor and not running
	UPROPERTY(EditAnywhere)
	bool bRegenerateMap;

	UPROPERTY(EditAnywhere) //old
	UBiomesComponent* Biomes; //old no longer use I guess

	UPROPERTY(EditAnywhere)
	class UBiomeGenerationComponent* BiomeGeneration;
private:
	float GenerateHeight(int32 XPosition, int32 YPosition);


	UPROPERTY(EditAnywhere, Category = "FBM")//number of perlin noise maps to layer
	int32 Octaves; 
	UPROPERTY(EditAnywhere, Category = "FBM")//the gap in frequency from one step to the next
	float Lacunarity; 
	UPROPERTY(EditAnywhere, Category = "FBM")//how the amplitude changes over time
	float Grain; 

	float FractalBrownianMotion(int32 XPosition, int32 YPosition);

	UPROPERTY(EditAnywhere, Category = "Seed")
	int32 Seed;
	//the random values to offset the perlin noise by in each octave, in order to introduce randomness
	TArray<float> OcataveOffsets;

	UPROPERTY(EditAnywhere, Category = "Seed")//if true will give a random seed on each generation
	bool bRandomSeed; 
	void GenerateSeed();
	FRandomStream Stream; //used in order to generate random numbers based on a specific seed

	float DomainWarping(float XPos, float YPos);
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")
	bool bDoDomain;
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")
	float DomainAmount;

	float SquareGradient(float XPosition, float YPosition);
	UPROPERTY(EditAnywhere, Category = "FallOff")//How sharp the transition will be, smaller means less abrupt
	float Steepness;
	UPROPERTY(EditAnywhere, Category = "FallOff") //controls how much of the area is actually underwater
	float Size;
	UPROPERTY(EditAnywhere, Category = "FallOff") //controls how much of the area is actually underwater
	float AboveWater;
	UPROPERTY(EditAnywhere, Category = "FallOff") //controls how much of the area is actually underwater
	bool bDoFalloff;


	UPROPERTY(EditAnywhere, Category = "Terraces") //controls how much of the area is actually underwater
	bool bDoTerracing;	
	UPROPERTY(EditAnywhere, Category = "Terraces") //controls how much of the area is actually underwater
	float TerraceSize;


	//TArray<FVector> TriangleNormals;
};

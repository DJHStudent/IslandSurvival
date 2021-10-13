// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BiomeGenerationComponent.h"
#include "ProcedurallyGeneratedTerrain.generated.h"

UCLASS()
class ISLANDSURVIVAL_API AProcedurallyGeneratedTerrain : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProcedurallyGeneratedTerrain();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)//number of vertices on x axis
	int32 Width; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)//number of vertices on y axis
	int32 Height; 
	UPROPERTY(EditAnywhere)//distance each vertex is appart
	float GridSize; 

	TArray<FVector> Vertices; //list of all vertices of mesh
	TArray<FLinearColor> VerticeColours; //the colour of each vertex
	TArray<int32> IslandNumber; //the key of the island each vertex relates to from the IslandsMap


	TArray<int32> Triangles; //list of all meshes trianlges

	UFUNCTION(BlueprintCallable)
	void RegenerateMap(); //when UI button pressed regenerate the map
	void CreateMesh();
	void ClearMap(); //remove any data stored for the previous map

	UPROPERTY(EditAnywhere)//the range of values perlin noise will return
	float PerlinScale; 
	UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
	float PerlinRoughness; 


	virtual bool ShouldTickIfViewportsOnly() const override; //update the maps terrain, when value changed even if in the editor and not running
	UPROPERTY(EditAnywhere)
	bool bRegenerateMap;

	UPROPERTY(EditAnywhere)
	class UBiomeGenerationComponent* BiomeGeneration; //class for holding all info related to the biomes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int32 Seed; //seed to use in the map for seeded generation
	FRandomStream Stream; //used in order to generate random numbers based on a specific seed
private:
	float GenerateHeight(int32 XPosition, int32 YPosition); //returns the height of each vertex in turn

	UPROPERTY(EditAnywhere, Category = "FBM")//number of perlin noise maps to layer
	int32 Octaves; 
	UPROPERTY(EditAnywhere, Category = "FBM")//the gap in frequency from one layer to the next
	float Lacunarity; 
	UPROPERTY(EditAnywhere, Category = "FBM")//how the amplitude changes over time
	float Grain; 

	float FractalBrownianMotion(int32 XPosition, int32 YPosition);

	//the random values to offset the perlin noise by in each octave, in order to introduce randomness
	TArray<float> OcataveOffsets;

	UPROPERTY(EditAnywhere, Category = "Seed")//if true will give a random seed on each generation
	bool bRandomSeed; 
	void GenerateSeed(); //determine the seed for the map

	float DomainWarping(float XPos, float YPos); //offset the vertices of each point by specific values
	UPROPERTY(EditAnyWhere, Category = "Domain Warping")//the amount of offset to actually apply to each vertex
	float DomainAmount; 

	float SquareGradient(float XPosition, float YPosition);
	UPROPERTY(EditAnywhere, Category = "FallOff")//steepness of the transition from island to deep abyss underwater
	float Steepness;
	UPROPERTY(EditAnywhere, Category = "FallOff") //controls how much of the map's border is actually underwater
	float Size;
	UPROPERTY(EditAnywhere, Category = "FallOff") //amound of the map, from centre point is above water
	float AboveWater;
	
	UPROPERTY(EditAnywhere, Category = "Terraces") //the distance appart of each terrace, larger value means smaller appart
	float TerraceSize;

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

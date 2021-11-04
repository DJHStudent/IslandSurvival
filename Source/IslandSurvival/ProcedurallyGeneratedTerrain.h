// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BiomeGenerationComponent.h"
#include "TerrainHeight.h"
#include "MainGameState.h"
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

	//need to be replicated variables
	TArray<FVector> Vertices; //list of all vertices of mesh
	TArray<FLinearColor> VerticeColours; //the colour of each vertex
	TArray<int32> IslandNumber; //the key of the island each vertex relates to from the IslandsMap


	TArray<int32> Triangles; //list of all meshes trianlges

	//UFUNCTION(NetMulticast, Reliable)
	void RegenerateMap(int32 tSeed, int32 tWidth, int32 tHeight, FRandomStream tStream); //when UI button pressed regenerate the map
	void CreateMesh();
	void ClearMap(); //remove any data stored for the previous map

	////UPROPERTY(EditAnywhere)//the range of values perlin noise will return
	////float PerlinScale; 
	////UPROPERTY(EditAnywhere)//how spiky or smooth the terrain will be
	////float PerlinRoughness; 


	virtual bool ShouldTickIfViewportsOnly() const override; //update the maps terrain, when value changed even if in the editor and not running
	//UPROPERTY(EditAnywhere)
	//bool bRegenerateMap;

	UPROPERTY(EditAnywhere)
	class UBiomeGenerationComponent* BiomeGeneration; //class for holding all info related to the biomes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int32 Seed; //seed to use in the map for seeded generation

	UPROPERTY() //once got value from server will generate the map
	FRandomStream Stream; //used in order to generate random numbers based on a specific seed

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

	UPROPERTY(EditAnywhere, Category = "Seed")//if true will give a random seed on each generation
	bool bRandomSeed; 
	void GenerateSeed(); //determine the seed for the map

	class AMainGameState* GameState;
	void SpawnMap();

	bool bTerrainGenerated;
};

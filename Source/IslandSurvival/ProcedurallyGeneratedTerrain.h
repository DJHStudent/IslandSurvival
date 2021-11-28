// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "BiomeGenerationComponent.h"
#include "TerrainHeight.h"
#include "MainGameState.h"
#include "Async/AsyncWork.h"
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
	UPROPERTY(EditAnywhere)
	bool bSmoothTerrain; //should all the biomes / terrain using terracing actually do it

	//need to be replicated variables
	TArray<FVector> Vertices; //list of all vertices of mesh
	TArray<FLinearColor> VerticeColours; //the colour of each vertex
	TArray<int32> IslandNumber; //the key of the island each vertex relates to from the IslandsMap


	TArray<int32> Triangles; //list of all meshes trianlges

	void RegenerateMap(int32 tSeed, int32 tWidth, int32 tHeight, FRandomStream tStream, bool tbSmoothTerrain); //when UI button pressed regenerate the map
	void RegenContinued();
	void CreateMesh();
	void ClearMap(); //remove any data stored for the previous map

	virtual bool ShouldTickIfViewportsOnly() const override; //update the maps terrain, when value changed even if in the editor and not running
	UPROPERTY(EditAnywhere)
	bool bRegenerateMap;

	UPROPERTY(EditAnywhere)
	class UBiomeGenerationComponent* BiomeGeneration; //class for holding all info related to the biomes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")
	int32 Seed; //seed to use in the map for seeded generation

	UPROPERTY() //once got value from server will generate the map
	FRandomStream Stream; //used in order to generate random numbers based on a specific seed

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AActor> ZombieSpawner;
	UPROPERTY(EditAnywhere)//the mesh to spawn in
	TSubclassOf<class AActor> Fuel;
private:
	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain Height")
	UTerrainHeight* TerrainHeight; //functionality for determining terrains height

	UPROPERTY(EditAnywhere, Category = "Seed")//if true will give a random seed on each generation
	bool bRandomSeed; 
	void GenerateSeed(); //determine the seed for the map

	FAsyncTask<class AsyncTerrainGeneration>* AsyncVertices;
	void GenerateMeshes();

	class AMainGameState* GameState;
};

class AsyncTerrainGeneration : public FNonAbandonableTask //generate the initial vertices of the terrain async
{
public:
	AProcedurallyGeneratedTerrain* Terrain;
	AsyncTerrainGeneration(AProcedurallyGeneratedTerrain* Terrain)
	{
		//constructor called
		this->Terrain = Terrain;
	}
	~AsyncTerrainGeneration()
	{
		//destructor called automatically when DoWork() done
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncTerrainGeneration, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork()
	{
		if (Terrain)
			Terrain->CreateMesh(); //async updating of the terrain
	}
};
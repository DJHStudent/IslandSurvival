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
	
	UFUNCTION(CallInEditor, Category = "Generate Terrain") //Generate new terrain
	void GenerateNewTerrain();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Seed")//the current seed of the terrain
	int32 Seed; 
		
	UPROPERTY(EditAnywhere, Category = "Override Settings") //Should all terrain biomes, regardless if using terracing, be smooth 
	bool bSmoothTerrain;
	UPROPERTY(EditAnywhere, Category = "Override Settings") //Should the biome spawning be overriden so only the base terrain is shown
	bool bOverrideBiomeSpawning;

	UPROPERTY(VisibleAnywhere, Category = "Other") //Save the terrain mesh as an asset
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Size")// for the mesh, the number of vertices on x axis
	int32 Width; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Terrain Size")//for the mesh, the number of vertices on y axis
	int32 Height; 
	UPROPERTY(EditAnywhere, Category = "Terrain Size")//distance each vertex is appart, on the mesh
	float GridSize; 


	//need to be replicated variables
	TArray<FVector> Vertices; //list of all vertices of mesh
	TArray<FLinearColor> VerticeColours; //the colour of each vertex
	TArray<int32> FeatureNumber; //the key of each island/lake and the vertex it relates to


	TArray<int32> Triangles; //list of all meshes trianlges

	void RegenerateMap(int32 tSeed, int32 tWidth, int32 tHeight, FRandomStream tStream, bool tbSmoothTerrain); //when UI button pressed regenerate the map
	void RegenContinued();
	void CreateMesh();
	void ClearMap(); //remove any data stored for the previous map

	//update the maps terrain, when value changed even if in the editor and not running
	virtual bool ShouldTickIfViewportsOnly() const override; 

	UPROPERTY(EditAnywhere, Category = "Other")//class for holding all info related to the biomes
	class UBiomeGenerationComponent* BiomeGeneration; 

	UPROPERTY() //once got value from server will generate the map, used in order to generate random numbers based on a specific seed
	FRandomStream Stream; 

	UPROPERTY(EditAnywhere, Category = "Other") //the Zombie Spawner blueprint to spawn in
	TSubclassOf<class AActor> ZombieSpawner;
	UPROPERTY(EditAnywhere, Category = "Other")//the Fuel Blueprint to spawn in
	TSubclassOf<class AActor> Fuel;

	bool bIsEditor;	
	
	UPROPERTY(EditAnywhere, Instanced, Category = "Terrain Height") //the base generation settings, used to determine the biomes locations
	UTerrainHeight* TerrainHeight;
private:
	UPROPERTY(EditAnywhere, Category = "Other") //the Zombie Spawner blueprint to spawn in
	UMaterialInterface* TerrainMaterial;

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
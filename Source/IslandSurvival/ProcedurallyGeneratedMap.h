 //Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProcedurallyGeneratedMap.generated.h"

UCLASS()
class ISLANDSURVIVAL_API AProcedurallyGeneratedMap : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProcedurallyGeneratedMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	int32 Width; //number of grid squares in X
	UPROPERTY(EditAnywhere)
	int32 Height; //number of grid squares in Y
	UPROPERTY(EditAnywhere)
	float GridSize; //size of each grid square

	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVCoords; //coordinates to specify how a texture will be applied

	//texturing arrays
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;

	UFUNCTION(BlueprintCallable)
	void GenerateMap();
	void ClearMap();

	UPROPERTY(EditAnywhere)
	float PerlinScale; //the range of values perlin noise will return
	UPROPERTY(EditAnywhere)
	float PerlinRoughness; //how spiky or smooth the terrain will be

	virtual bool ShouldTickIfViewportsOnly() const override; //update the maps terrain, when value changed even if in the editor and not running
	UPROPERTY(EditAnywhere)
	bool bRegenerateMap;

private:
	UPROPERTY(EditAnywhere, Category = "FBM")
	int32 Octaves; //number of perlin noise maps to layer
	UPROPERTY(EditAnywhere, Category = "FBM")
	float Lacunarity; //the gap in frequency from one step to the next
	UPROPERTY(EditAnywhere, Category = "FBM")
	float Grain; //how the amplitude changes over time

	UPROPERTY(EditAnyWhere)
	float seed;

	TArray<float> OcataveOffset;

	float CalculateHeight(float XPosition, float YPosition, float PerlinOffset); //calculates the specific height value for each position using FBM combined with Perlin Noise
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "MainGameState.generated.h"

/**
 the state of the current level / game in
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMainGameState();
	void GenerateTerrain(int32 Seed, int32 Width, int32 Height);
private:
	AProcedurallyGeneratedTerrain* ProceduralTerrain;
};

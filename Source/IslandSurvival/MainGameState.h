// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated)
	int32 TerrainWidth;
	UPROPERTY(Replicated)
	int32 TerrainHeight;
	UPROPERTY(Replicated)
	int32 Seed;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
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

	UPROPERTY(ReplicatedUsing = UpdateWidth)
	int32 TerrainWidth;
	UPROPERTY(ReplicatedUsing = UpdateHeight)
	int32 TerrainHeight;
	UPROPERTY(ReplicatedUsing = UpdateSeed)
	int32 Seed;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated	

private:

	UFUNCTION()
		void UpdateSeed();	
	UFUNCTION()
		void UpdateWidth();	
	UFUNCTION()
		void UpdateHeight();
};

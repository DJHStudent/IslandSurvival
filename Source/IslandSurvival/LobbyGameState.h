// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
public:
	ALobbyGameState();

	//when these change on the servers version, properly update each client as well
	UPROPERTY(ReplicatedUsing = UpdateWidth)
		int32 TerrainWidth;
	UPROPERTY(ReplicatedUsing = UpdateHeight)
		int32 TerrainHeight;
	UPROPERTY(ReplicatedUsing = UpdateSeed)
		int32 Seed;
	UPROPERTY(ReplicatedUsing = UpdateSmooth)
		bool bSmoothTerrain;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated	

private:

	UFUNCTION()
		void UpdateSeed();
	UFUNCTION()
		void UpdateWidth();
	UFUNCTION()
		void UpdateHeight();
	UFUNCTION()
		void UpdateSmooth();

};


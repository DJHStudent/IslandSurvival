// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated	

	void GenerateTerrain(int32 Seed, int32 Width, int32 Height);

	UPROPERTY(ReplicatedUsing = HasStreamRepliacted)
	FRandomStream Stream;
	UPROPERTY(ReplicatedUsing = UpdateWidth)
		int32 TerrainWidth;
	UPROPERTY(ReplicatedUsing = UpdateHeight)
		int32 TerrainHeight;
	UPROPERTY(ReplicatedUsing = UpdateSeed)
		int32 TerrainSeed;

	bool bStreamRep;
	bool bSeedRep;
	bool bWidthRep;
	bool bHeightRep;

private:
	void CalculateSeed(int32 Seed);

	UFUNCTION()
		void UpdateSeed();
	UFUNCTION()
		void UpdateWidth();
	UFUNCTION()
		void UpdateHeight();



	UFUNCTION(NetMultiCast, Reliable)
	void MakeMap();
	void EnsureReplicated();





	UFUNCTION()
	void HasStreamRepliacted();
	UFUNCTION()
	void HasSeedRepliacted();
	UFUNCTION()
	void HasWidthRepliacted();
	UFUNCTION()
	void HasHeightRepliacted();

	int32 TempWidth;
	int32 TempHeight;
	int32 TempSeed;
};

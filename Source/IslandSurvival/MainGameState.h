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

	void GenerateTerrain(int32 Seed, int32 Width, int32 Height); //call to setup the values

	UPROPERTY(ReplicatedUsing = HasStreamRepliacted)
	FRandomStream Stream;
	UPROPERTY(ReplicatedUsing = HasSeedRepliacted)
	int32 TerrainSeed;
	UPROPERTY(ReplicatedUsing = HasWidthRepliacted)
	int32 TerrainWidth;
	UPROPERTY(ReplicatedUsing = HasHeightRepliacted)
	int32 TerrainHeight;

	bool bStreamRep;
	UPROPERTY(Replicated)
		bool bSeedRep;
	bool bWidthRep;
	bool bHeightRep;

	UFUNCTION(Server, Reliable)
		void EnsureReplicated();

	UPROPERTY(ReplicatedUsing = HasUIRepliacted)
		bool bUIRep;


	//UFUNCTION()
	//	void UpdatePlayerUI();	
	UFUNCTION(Client, Reliable)
		void UpdatePlayerUI();

	void CalculateSeed();
private:
	UFUNCTION(NetMulticast, Reliable)
	void MakeMap();

	UFUNCTION()
	void HasUIRepliacted();	
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

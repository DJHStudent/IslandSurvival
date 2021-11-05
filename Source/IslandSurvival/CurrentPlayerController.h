// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainGameInstance.h"
#include "CurrentPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ACurrentPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void ServerInitilizeTerrain(); //on the server, this function gets called
		void ClientUpdateUI(); //Called on server, finds client whichs owns it and runs the code

	void ServerUpdateTerrain(int32 Seed, int32 Width, int32 Height, FRandomStream Stream);
	UFUNCTION(Client, Reliable)
	void ClientUpdateTerrain(int32 Seed, int32 Width, int32 Height, FRandomStream Stream);
	bool bTerrainUpdated;

	void ServerUpdateColour(UMaterialInterface* Colour); //on the server, this function gets called
	UFUNCTION(Client, Reliable)
	void ClientUpdateColour(UMaterialInterface* Colour); //Called on server, finds client whichs owns it and runs the code

	UMainGameInstance* MainGameInstance;
	//probably something in here to actually get the terrain generating properly
};

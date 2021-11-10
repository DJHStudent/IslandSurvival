// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameState.h"
#include "CurrentPlayerController.h"
#include "MainGameState.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	void UpdateTerrainValues(int32 Seed, int32 Width, int32 Height, bool bSmoothTerrain);

private:
	virtual void PostSeamlessTravel() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;


	AMainGameState* MainGameState;
	ACurrentPlayerController* HostController;
};

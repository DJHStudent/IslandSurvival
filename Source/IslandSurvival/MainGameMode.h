// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameState.h"
#include "CurrentPlayerController.h"
#include "MainGameState.h"
#include "PlayerCharacter.h"
#include "Animation/SkeletalMeshActor.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameMode : public AGameMode
{ //used when server is in the terrain map
	GENERATED_BODY()
public:
	void UpdateTerrainValues(int32 Seed, int32 Width, int32 Height, bool bSmoothTerrain);
	void PlayerDeath(APlayerCharacter* Player);
private:
	AMainGameMode();

	virtual void PostSeamlessTravel() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
};

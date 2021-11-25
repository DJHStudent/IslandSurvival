// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ALobbyGameMode : public AGameMode
{ //used when server is in the lobby map
	GENERATED_BODY()

public:
	void PlayerDeath(AActor* Player);

private:
	ALobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;


	UMaterialInterface* HostColour;
	TArray<UMaterialInterface*> PlayerColours; //list of all possible colours player can still be when joining session
	TMap<APlayerController*, UMaterialInterface*> UsedPlayerColours; //list of used colours and their related PlayerController

	FVector SpawnLocation;
};

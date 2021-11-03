// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CurrentPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ACurrentPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void ServerUpdateUI();
	UFUNCTION(Client, Reliable)
		void ClientUpdateUI();
};

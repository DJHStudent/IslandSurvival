// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "MainGameState.h"
#include "Engine/World.h"
#include "CurrentPlayerController.h"
#include "Kismet/GameplayStatics.h"

void AMainGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages)
{
	Super::InitGame(MapName, Options, ErrorMessages);

	//GameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

void AMainGameMode::PostSeamlessTravel() //once seamless travel all complete, then can actually load the map
{
	Super::PostSeamlessTravel();

	UE_LOG(LogTemp, Error, TEXT("Seamless travel has been completed"))
	AMainGameState* MainGameState = GetGameState<AMainGameState>();
	if (MainGameState)
	{
		MainGameState->CalculateSeed();
		//MainGameState->UpdatePlayerUI();
		MainGameState->PlayerArray;
	}
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //for all players of level
	{
		ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(It->Get());
		if(PlayerController)
		{ 
			PlayerController->ServerUpdateUI();
		}
	}
	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Engine/World.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "Kismet/GameplayStatics.h"
void AMainGameMode::PostSeamlessTravel() //once seamless travel all complete for all clients, then can actually load the map
{
	Super::PostSeamlessTravel();
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //for all players which have appeared in this new level
	{
		ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(It->Get()); //get the controller
		if (PlayerController && PlayerController->GetLocalRole() == ROLE_Authority && PlayerController->IsLocalController()) //find controller on server only
		{
			PlayerController->ServerInitilizeTerrain(); //get the terrain values from the host client
		}
	}
}

void AMainGameMode::UpdateTerrainValues(int32 Seed, int32 Width, int32 Height, bool bSmoothTerrain) //tell each player to actually update their terrain
{
	FRandomStream Stream; //determine the seed all players will use
	if (Seed == 0)
	{
		Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
		Seed = Stream.GetCurrentSeed(); //assign the seed the streams seed
	}
	else
	{
		Stream.Initialize(Seed);
		Seed = Seed;
	}

	for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //for all players which have appeared in this new level
	{
		ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(It->Get()); //get the controller
		if (PlayerController && PlayerController->HasActorBegunPlay()) //if found, update the UI to use the player HUD widget, as well as their terrain
		{
			PlayerController->ServerUpdateTerrain(Seed, Width, Height, Stream, bSmoothTerrain);
		}
	}
}

void AMainGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{ //called just as soon as connection occurs with this server
	ErrorMessage = TEXT("Failed to Login"); //auto fail as on the wrong map, can only join if on lobby

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage); //broadcast to the player this failure
}

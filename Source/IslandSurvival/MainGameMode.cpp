// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Engine/World.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "Kismet/GameplayStatics.h"

void AMainGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages)
{
	Super::InitGame(MapName, Options, ErrorMessages);
	PrimaryActorTick.bCanEverTick = false;
	//GameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
}

void AMainGameMode::PostSeamlessTravel() //once seamless travel all complete, then can actually load the map
{
	Super::PostSeamlessTravel();

	UE_LOG(LogTemp, Error, TEXT("Seamless travel has been completed"))
	MainGameState = GetGameState<AMainGameState>();
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It) //for all players which have appeared in this new level
	{
		ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(It->Get()); //get the controller
		if(PlayerController) //if found, update the UI to use the one for this level
		{ 
			if (PlayerController->GetLocalRole() == ROLE_Authority && PlayerController->IsLocalController()) //find controller on server and update it
			{
				HostController = PlayerController;
				PlayerController->ServerInitilizeTerrain();
			}
		}
	}
}

void AMainGameMode::UpdateTerrainValues(int32 Seed, int32 Width, int32 Height)
{
	FRandomStream Stream;
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
		if (PlayerController && PlayerController->HasActorBegunPlay()) //if found, update the UI to use the one for this level
		{
			PlayerController->ServerUpdateTerrain(Seed, Width, Height, Stream);
		}
	}
}

void AMainGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ErrorMessage = TEXT("Failed to Login"); //auto fail as on the wrong map

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}

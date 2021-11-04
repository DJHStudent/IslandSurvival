// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Engine/World.h"
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
			PlayerController->ServerUpdateUI();
			if (PlayerController->GetLocalRole() == ROLE_Authority)
				HostController = PlayerController;
		}
	}
	////float RepWaitTime = 10.0f;
	////FTimerHandle Timer; //timer to handle spawning of player after death
	////GetWorldTimerManager().SetTimer(Timer, this, &AMainGameMode::UpdateTerrain, RepWaitTime, false);
	////
}

void AMainGameMode::HandleMatchHasStarted() //once seamless travel all complete, then can actually load the map
{
	AGameMode::HandleMatchHasStarted();
	////////ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(HostController); //get the controller
	////////HostController->ServerUpdateTerrain();
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
		if (PlayerController) //if found, update the UI to use the one for this level
		{
			PlayerController->ServerUpdateTerrain(Seed, Width, Height, Stream);
		}
	}
}
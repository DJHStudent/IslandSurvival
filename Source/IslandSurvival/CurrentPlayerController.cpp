// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrentPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "PlayerGameHUD.h"
#include "Engine/Engine.h"
#include "ProcedurallyGeneratedTerrain.h"
#include "TimerManager.h"

ACurrentPlayerController::ACurrentPlayerController()
{
	NetUpdateFrequency = 100;
}

void ACurrentPlayerController::BeginPlay()
{
	Super::BeginPlay();
}


void ACurrentPlayerController::ServerInitilizeTerrain() //on the server controller update the terrain
{
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance) //get the hosts game instance
		MainGameInstance->TerrainToServer(); //put their terrain values onto the MainGameMode class
}

void ACurrentPlayerController::ClientUpdateUI() //update the clients UI
{
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance) //get the clients GameInstance
		MainGameInstance->LoadGame(); //as called when game starts, update UI to be the players HUD
}

void ACurrentPlayerController::ServerUpdateTerrain(int32 Seed, int32 Width, int32 Height, FRandomStream Stream, bool bSmoothTerrain)
{
	ClientUpdateTerrain(Seed, Width, Height, Stream, bSmoothTerrain); //as on server, call the related client to update its terrain
}

void ACurrentPlayerController::ClientUpdateTerrain_Implementation(int32 Seed, int32 Width, int32 Height, FRandomStream Stream, bool bSmoothTerrain)
{ //RPC down to the client so it can update its UI and terrain
	ClientUpdateUI(); //just before the terrain loads in update the UI
	
	TryUpdateTerrain(Seed, Width, Height, Stream, bSmoothTerrain); //renerate it, using these values 
	//update clients UI with seed
	if (MainGameInstance && MainGameInstance->CurrentPlayerHUDWidget)
	{
		MainGameInstance->CurrentPlayerHUDWidget->UpdateSeedTextBlock(FString::FromInt(Seed)); //update players UI to display this seed
	}
}




void ACurrentPlayerController::TryUpdateTerrain(int32 Seed, int32 Width, int32 Height, FRandomStream Stream, bool bSmoothTerrain)
{ //repeatedly try to update the terrain, attempting again if somehow it failed to actually find the terrain actor in the world
	AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	if (ProceduralTerrain) //if found terrain actor in scene, note sometimes actually fails to find the terrain at all
		ProceduralTerrain->RegenerateMap(Seed, Width, Height, Stream, bSmoothTerrain); //renerate it, using these values 
	else //terrain not loaded in yet, I guess, so try again
	{		
		float WaitTime = 0.5f; //wait 1/2 second and try loading in again
		//code to run the timer
		FTimerHandle RetryTimer;
		FTimerDelegate RetryDelegate = FTimerDelegate::CreateUObject(this, &ACurrentPlayerController::TryUpdateTerrain,
			Seed, Width, Height, Stream, bSmoothTerrain); //custom delegate to allow paramater to be passed in
		GetWorldTimerManager().SetTimer(RetryTimer, RetryDelegate, WaitTime, false);
	}
}

void ACurrentPlayerController::ServerUpdateColour(UMaterialInterface* Colour)
{ //called so each client can store reference to colour material they are using
	ClientUpdateColour(Colour); //as on server, move down to the clients version
}

void ACurrentPlayerController::ClientUpdateColour_Implementation(UMaterialInterface* Colour)
{
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance) //get the clients GameInstance
		MainGameInstance->PlayerColour = Colour; //store the players colour material using in the GameInstance class
}
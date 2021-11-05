// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrentPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "PlayerGameHUD.h"
#include "ProcedurallyGeneratedTerrain.h"

void ACurrentPlayerController::ServerInitilizeTerrain() //on the server controller update the terrain
{
	//ClientUpdateUI();
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance)
		MainGameInstance->UpdateTerrain();
	////if (GetLocalRole() == ROLE_Authority) //if one which is on the server, can actually call the code to update the terrain
	////{
	////	UE_LOG(LogTemp, Error, TEXT("On This Player Updating its UI"))
	////	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	////	if (MainGameInstance)
	////		MainGameInstance->LoadGame();
	////}

}

void ACurrentPlayerController::ClientUpdateUI()
{
	UE_LOG(LogTemp, Error, TEXT("On This Player Updating its UI"))
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance)
		MainGameInstance->LoadGame();
	//AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	//if (ProceduralTerrain)
	//	ProceduralTerrain->RegenerateMap();
}

void ACurrentPlayerController::ServerUpdateTerrain(int32 Seed, int32 Width, int32 Height, FRandomStream Stream)
{
	ClientUpdateTerrain(Seed, Width, Height, Stream);
}

void ACurrentPlayerController::ClientUpdateTerrain_Implementation(int32 Seed, int32 Width, int32 Height, FRandomStream Stream)
{
	ClientUpdateUI(); //just before the terrain loads in update the UI
	UE_LOG(LogTemp, Warning, TEXT("Client beginning Process to make terrain"))
	AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	if (ProceduralTerrain)
		ProceduralTerrain->RegenerateMap(Seed, Width, Height, Stream);
	//update clients UI with seed
	//MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance && MainGameInstance->CurrentPlayerHUDWidget)
	{
		MainGameInstance->CurrentPlayerHUDWidget->UpdateSeedTextBlock(FString::FromInt(Seed));
	}
}

void ACurrentPlayerController::ServerUpdateColour(UMaterialInterface* Colour)
{
	ClientUpdateColour(Colour);
	////////if (GetLocalRole() == ROLE_Authority) //if one which is on the server, can actually call the code to update the terrain
	////////{
	////////	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	////////	if (MainGameInstance)
	////////		MainGameInstance->PlayerColour = Colour;
	////////}

}

void ACurrentPlayerController::ClientUpdateColour_Implementation(UMaterialInterface* Colour)
{
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance)
		MainGameInstance->PlayerColour = Colour;
	//AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	//if (ProceduralTerrain)
	//	ProceduralTerrain->RegenerateMap();
}
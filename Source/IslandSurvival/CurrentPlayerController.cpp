// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrentPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"

void ACurrentPlayerController::ServerUpdateUI()
{
	ClientUpdateUI();
	if (GetLocalRole() == ROLE_Authority) //if one which is on the server, can actually call the code to update the terrain
	{
		UE_LOG(LogTemp, Error, TEXT("On This Player Updating its UI"))
			UMainGameInstance* MainGameInstancet = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (MainGameInstancet)
			MainGameInstancet->LoadGame();
	}

}

void ACurrentPlayerController::ClientUpdateUI_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("On This Player Updating its UI"))
		UMainGameInstance* MainGameInstancet = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstancet)
		MainGameInstancet->LoadGame();
}
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
			//PlayerController->ServerUpdateUI(); //only call the update on the server
			if (PlayerController->GetLocalRole() == ROLE_Authority && PlayerController->IsLocalController()) //find controller on server and update it
			{
				HostController = PlayerController;
				PlayerController->ServerInitilizeTerrain();
			}
		}
	}
	////float RepWaitTime = 10.0f;
	////FTimerHandle Timer; //timer to handle spawning of player after death
	////GetWorldTimerManager().SetTimer(Timer, this, &AMainGameMode::UpdateTerrain, RepWaitTime, false);
	////
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

	//////as the terrain has successfully been generated in, at least on the server version can spawn in the enemy spawners
	////AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	////if (ProceduralTerrain)
	////{
	////	for (auto& IslandPair : ProceduralTerrain->BiomeGeneration->IslandPointsMap)
	////	{
	////		float IslandWidths = (IslandPair.Value.MaxXPosition - IslandPair.Value.MinXPosition);
	////		float IslandHeights = (IslandPair.Value.MaxYPosition - IslandPair.Value.MinYPosition);

	////		//determine the actual size of the rectangular grid covering the island by using its min and max position * by grid size so its their actual real size
	////		float IslandWidth = (IslandPair.Value.MaxXPosition - IslandPair.Value.MinXPosition) * ProceduralTerrain->GridSize;
	////		float IslandHeight = (IslandPair.Value.MaxYPosition - IslandPair.Value.MinYPosition) * ProceduralTerrain->GridSize;

	////		//use possion disk sampling for each island to determine optimal spawn location so get an even distribution
	////		//AActor* SpawnedSpawner = GetWorld()->SpawnActor<AActor>(ProceduralTerrain->ZombieSpawner, FVector(100, 100, 0), FRotator::ZeroRotator);
	////	}
	////}
}
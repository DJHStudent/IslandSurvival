// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"

AMainGameState::AMainGameState()
{
	ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
}

void AMainGameState::GenerateTerrain(int32 Seed, int32 Width, int32 Height)
{
	if (ProceduralTerrain)
	{
		ProceduralTerrain->Seed = Seed;
		ProceduralTerrain->Width = Width;
		ProceduralTerrain->Height = Height;

		ProceduralTerrain->RegenerateMap();

		/* Steps on how the terrain should actually get generated
			1. Generate Vertices Array on Server
			2. Move the Vertices Array to each and every client
			3. Using the Seed I Guess can do the rest

			Or
			1. Pass in the seed using as determined by the server, then do 100% of rest on the clients and it will be the right stuff

		*/
	}
}
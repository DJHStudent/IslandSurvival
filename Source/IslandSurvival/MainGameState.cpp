// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "ProcedurallyGeneratedTerrain.h"

AMainGameState::AMainGameState()
{
	///////*bStreamRep = false;
	//////bHeightRep = false;
	//////bWidthRep = false;
	//////bSeedRep = false;*/
}
void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, TerrainWidth);
	DOREPLIFETIME(AMainGameState, TerrainHeight);
	DOREPLIFETIME(AMainGameState, TerrainSeed);
	DOREPLIFETIME(AMainGameState, Stream);
}
//both these 2 functions only actually get called once host presses start button, so only ever called on server
void AMainGameState::GenerateTerrain(int32 Seed, int32 Width, int32 Height)
{
	TempWidth = Width;
	//bWidthRep = true;
	TempHeight = Height;
	//bHeightRep = true;
	TempSeed = Seed;
	//CalculateSeed(Seed);

	//GetWorld()->HasBegunPlay();

	//ProceduralTerrain->RegenerateMap();

	/* Steps on how the terrain should actually get generated
		1. Generate Vertices Array on Server
		2. Move the Vertices Array to each and every client
		3. Using the Seed I Guess can do the rest

		Or
		1. Pass in the seed using as determined by the server, then do 100% of rest on the clients and it will be the right stuff

	*/
}

void AMainGameState::EnsureReplicated()
{
	if (GetWorld()->HasBegunPlay()) //only replicate if all actors actually spawned in and started
	{
		TerrainWidth = TempWidth;
		bWidthRep = true;
		TerrainHeight = TempHeight;
		bHeightRep = true;
		TerrainSeed = TempSeed;
		CalculateSeed(TerrainSeed);
	}
	else
	{
		float RepWaitTime = 1.0f;
		FTimerHandle Timer; //timer to handle spawning of player after death
		GetWorldTimerManager().SetTimer(Timer, this, &AMainGameState::EnsureReplicated, RepWaitTime, false);
	}
}

void AMainGameState::CalculateSeed(int32 Seed)
{
	if (Seed == 0)
	{
		Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
		TerrainSeed = Stream.GetCurrentSeed(); //assign the seed the streams seed
	}
	else
	{
		Stream.Initialize(Seed);
		TerrainSeed = Seed;
	}
	bSeedRep = true;
	bStreamRep = true;
	////MakeMap();
	UE_LOG(LogTemp, Error, TEXT("Code to begin terrain gen actually gets called on clients: %i"), TerrainSeed)
}

void AMainGameState::MakeMap()
{
	//on server version find procedural terrain and spawn it in
	// 
	AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	if (ProceduralTerrain)
		ProceduralTerrain->RegenerateMap();
	//RegenerateMap(); //as on server 
}

void AMainGameState::HasStreamRepliacted()
{
	bStreamRep = true;
}

void AMainGameState::HasSeedRepliacted()
{
	bStreamRep = true;
}

void AMainGameState::HasWidthRepliacted()
{
	bWidthRep = true;
}

void AMainGameState::HasHeightRepliacted()
{
	bHeightRep = true;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "MainGameInstance.h"
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
	DOREPLIFETIME(AMainGameState, bSeedRep);
	DOREPLIFETIME(AMainGameState, bUIRep);
}
//both these 2 functions only actually get called once host presses start button, so only ever called on server
void AMainGameState::GenerateTerrain(int32 Seed, int32 Width, int32 Height)
{
	//TerrainWidth = Width;
	//bWidthRep = true;
	//TerrainHeight = Height;
	//bHeightRep = true;
	////TerrainSeed = Seed;
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

void AMainGameState::EnsureReplicated_Implementation()
{
	TerrainWidth = TerrainWidth;
	bWidthRep = bWidthRep;
	TerrainHeight = TerrainHeight;
	bHeightRep = bHeightRep;
	TerrainSeed = TerrainSeed;
	bSeedRep = bSeedRep;
	Stream = Stream;
	bStreamRep = bStreamRep;
}

void AMainGameState::CalculateSeed(int32 Seed)
{
	//if (Seed == 0)
	//{
	//	Stream.GenerateNewSeed(); //this generates us a new random seed for the stream
	//	TerrainSeed = Stream.GetCurrentSeed(); //assign the seed the streams seed
	//}
	//else
	//{
	//	Stream.Initialize(Seed);
	//	TerrainSeed = Seed;
	//}
	//bSeedRep = true;
	//bStreamRep = true;
	//MakeMap();
	//UE_LOG(LogTemp, Error, TEXT("Code to begin terrain gen actually gets called on clients: %i"), TerrainWidth)
}

void AMainGameState::MakeMap_Implementation()
{
	//on server version find procedural terrain and spawn it in
	// 
/*	UE_LOG(LogTemp, Warning, TEXT("Yep, Netmulticasts do actually work on the Terrain"))
	AProcedurallyGeneratedTerrain* ProceduralTerrain = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass()));
	*///if (ProceduralTerrain)
	//	ProceduralTerrain->RegenerateMap();
	//RegenerateMap(); //as on server 
}

void AMainGameState::HasUIRepliacted()
{
	UE_LOG(LogTemp, Error, TEXT("On This Player Updating its UI"))
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

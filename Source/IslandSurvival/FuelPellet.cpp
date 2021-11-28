// Fill out your copyright notice in the Description page of Project Settings.


#include "FuelPellet.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

// Sets default values
AFuelPellet::AFuelPellet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bCollided = false;
}

// Called when the game starts or when spawned
void AFuelPellet::BeginPlay()
{
	Super::BeginPlay();

	MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld())); //get a reference to the Game State using
	//add collision detection for the player
	OnActorBeginOverlap.AddDynamic(this, &AFuelPellet::OnOverlap);
}

void AFuelPellet::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{ //called when colliding with a player
	if (GetWorld()->IsServer())
	{
		//when collide call game state, update fuel value and replicate UI change down to all clients
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor); //if collision occured with the player
		if (MainGameState && Player && !bCollided)
		{
			bCollided = true;
			MainGameState->ServerAddFuel();
			//spawn in fade particle system


			Destroy(); //remove actor from world
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSpawner.h"

// Sets default values
AZombieSpawner::AZombieSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ZombieScale = 1;
	ZombieSwimSpeed = 1000;
	ZombieWalkSpeed = 600;
}

// Called when the game starts or when spawned
void AZombieSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}
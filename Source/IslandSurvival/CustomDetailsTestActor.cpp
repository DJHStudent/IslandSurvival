// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDetailsTestActor.h"

// Sets default values
ACustomDetailsTestActor::ACustomDetailsTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomDetailsTestActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACustomDetailsTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomDetailsTestActor::SetValues(FText ItemText)
{
	//LightOptionsEnum = ItemText.ToString();
	UE_LOG(LogTemp, Warning, TEXT("Text is being updated so also update the editor with the new values"))
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomDetailsTestActor.generated.h"

UCLASS()
class ISLANDSURVIVAL_API ACustomDetailsTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomDetailsTestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Lighting")
	bool bCastStaticShadow;
	UPROPERTY(EditAnywhere, Category = "Lighting")
	bool bCastDynamicShadow;
	UPROPERTY(EditAnywhere, Category = "Lighting")
	bool bCastVolumetricTranslucentShadow;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomDetailsTestActor.generated.h"

UENUM() 
namespace ELightOptions
{
	enum Type //this way so the enum can appear within the editor
	{
		StaticShadow UMETA(DisplayName = "Static"),
		DynamicShadow UMETA(DisplayName = "Dynamicness"),
		VolumetricTranslucentShadow UMETA(DisplayName = "Volumetric"),
	};
}

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
	int DynamicAmount;
	UPROPERTY(EditAnywhere, Category = "Lighting")
	FString VolumetricLightName;
	
	UPROPERTY(EditAnywhere, Category="Lighting")
	TEnumAsByte<ELightOptions::Type> LightOptionsEnum; //allows the enum to appear within the editor

	void SetValues(FText ItemText);
};

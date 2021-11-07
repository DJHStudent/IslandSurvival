// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h" //much be above the generated.h file
#include "ProcedurallyGeneratedTerrain.h"
#include "MainGameInstance.h"
#include "PlayerGameHUD.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class ISLANDSURVIVAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);
	void Strafe(float Value);
	void LookUp(float Value);
	void Turn(float Value);

	void SprintStart();
	void SprintEnd();
	UFUNCTION(Server, Reliable)
		void ServerSprintStart();
	UFUNCTION(Server, Reliable)
		void ServerSprintEnd();
	//the multiplayer functions for movement
	float SprintMovementSpeed;
	float NormalMovementSpeed;
	float SwimMovementSpeed;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintReload(); //a function which will be made and editied in blueprints but callable in C++ code
	void Reload(); //reload gun using blueprint nodes

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString CurrentBiomeText;

	void Paused();
	void Resume();

	void ShowMouseCursor();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all these variables to be replicated	
private: //i.e a member variable
	UPROPERTY(EditInstanceOnly)
	float LookSensitivity;

	UPROPERTY(EditInstanceOnly)
	float SprintMultiplier;

	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		//forward delcaration e.g class
	class UPlayerCharacterAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere)
	AProcedurallyGeneratedTerrain* BiomeList;
	UPlayerGameHUD* PlayerWidget;

	void DisplayPointBiome(); //displays the biome at the specified point

	UMainGameInstance* MainGameInstance;

	void UISetup();
	UFUNCTION(Server, Reliable)
	void ServerUpdatePlayerColour(UMaterialInterface* Colour);

	UPROPERTY(ReplicatedUsing = ReplicatedColourUpdate)
	UMaterialInterface* PlayersColour;
	UFUNCTION()
		void ReplicatedColourUpdate();

	void GenerateMap();

	bool bPaused;
};

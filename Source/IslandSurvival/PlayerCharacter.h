// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h" //much be above the generated.h file

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

	//the multiplayer functions for movement
	float SprintMovementSpeed;
	float NormalMovementSpeed;


	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintReload(); //a function which will be made and editied in blueprints but callable in C++ code
	void Reload(); //reload gun using blueprint nodes


private: //i.e a member variable
	UPROPERTY(EditInstanceOnly)
	float LookSensitivity;

	UPROPERTY(EditInstanceOnly)
	float SprintMultiplier;

	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		//forward delcaration e.g class
	class UPlayerCharacterAnimInstance* AnimInstance;
};
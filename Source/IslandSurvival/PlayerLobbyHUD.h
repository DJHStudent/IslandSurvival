// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "PlayerLobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API APlayerLobbyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	APlayerLobbyHUD();

private:
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	class ULobbyWidget* Lobby;


	void SetEditability(); //set conditions so only host can actually press / edit these elements

	UPROPERTY(meta = (BindWidget))
		UButton* ButtonLeave;
	UPROPERTY(meta = (BindWidget))
		UButton* ButtonStart;

	UPROPERTY(meta = (BindWidget))
		USpinBox* SpinBoxSeed;
	UPROPERTY(meta = (BindWidget))
		USpinBox* SpinBoxWidth;
	UPROPERTY(meta = (BindWidget))
		USpinBox* SpinBoxHeight;

	//class UMainGameInstance* MainGameInstance;

	UFUNCTION()
		void OnStartButtonPressed();
	UFUNCTION()
		void OnLeaveButtonPressed();
	UFUNCTION()
		void OnSeedChanged(float InValue); //when host changes seed
	UFUNCTION()
		void OnWidthChanged(float InValue); //when host changed width
	UFUNCTION()
		void OnHeightChanged(float InValue); //when host changes height
};

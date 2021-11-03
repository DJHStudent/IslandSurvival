// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "MainGameInstance.h"
#include "MainGameState.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetEditability(APawn* Player); //set conditions so only host can actually press / edit these elements

	//on every client these will be called to update the text whenever values change
	void SetSeed(int32 Value);
	void SetWidth(int32 Value);
	void SetHeight(int32 Value);
private:
	virtual bool Initialize() override;

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

	class UMainGameInstance* MainGameInstance;
	AMainGameState* LobbyGameState;

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

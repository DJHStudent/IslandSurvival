// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/CheckBox.h"
#include "MainGameInstance.h"
#include "LobbyGameState.h"
#include "Components/CanvasPanel.h"
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
	void SetSmooth(bool Value);

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DeathMessage;
private:
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget)) //assigns this element to the related one from the widget itself
	UButton* ButtonLeave;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonStart;
	
	UPROPERTY(meta = (BindWidget))
	USpinBox* SpinBoxSeed;
	UPROPERTY(meta = (BindWidget))
	USpinBox* SpinBoxWidth;
	UPROPERTY(meta = (BindWidget))
	USpinBox* SpinBoxHeight;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* CheckBoxSmooth;

	class UMainGameInstance* MainGameInstance;
	ALobbyGameState* LobbyGameState;

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
	UFUNCTION()
	void OnSmoothChanged(bool bIsChecked); //when host changes height
};

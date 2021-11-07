// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "MainGameInstance.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateJoinningText(FString Message);
private:
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget)) //auto binds this variable to a widegt element with the same name when widget using this class
	UButton* ButtonHost;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonJoin;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonQuit;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Loading;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonCancel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockJoining;

	class UMainGameInstance* MainGameInstance;

	UFUNCTION()
	void OnHostButtonPressed();
	UFUNCTION()
	void OnJoinButtonPressed();
	UFUNCTION()
	void OnQuitButtonPressed();	
	UFUNCTION()
	void OnCancelButtonPressed();
};

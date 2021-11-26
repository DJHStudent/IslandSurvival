// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "MainMenuWidget.h"
#include "LobbyWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "PlayerGameHUD.h"
#include "PlayerCharacter.h"
#include "MainGameInstance.generated.h"

UENUM()
enum class EGameState : uint8 //enum specifying the current state of a player
{
	LOBBY UMETA(DisplayName = "Lobby"),
	GAME UMETA(DisplayName = "Game"),
};
UCLASS()
class ISLANDSURVIVAL_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance(const FObjectInitializer& ObjectInitilize);
	virtual void Init(); //auto called when game starts

	UFUNCTION(BlueprintCallable)
	void LoadMenu(); //whenever mainmenu scene loads up call this function
	UFUNCTION(BlueprintCallable)
	void LoadLobby(APawn* Player);
	void PlayerDeathStart(class APlayerCharacter* PlayerActor);
	void PlayerDeathEnd(APlayerCharacter* PlayerActor);

	void HostSession();
	void JoinSession();

	void StartGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();

	void TerrainToServer();
	void FinishTerrainLoading();

	void CancelFindSession();
	
	class ULobbyWidget* Lobby;
	UPlayerGameHUD* CurrentPlayerHUDWidget;

	EGameState CurrentGameState;

	//these variables will only actually be on the server version, used to store values when doing a server travel
	int32 TerrainWidth;
	int32 TerrainHeight;
	int32 Seed;
	bool bSmoothTerrain;

	void QuitLobby();

	UMaterialInterface* PlayerColour; //material holding the colour of the player

	void NetworkCrash(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailtureType, const FString& ErrorString);

	bool bCrashed; //did network just crash or not
private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	class UMainMenuWidget* MainMenu;
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	
	TSubclassOf<UUserWidget> PlayerHUDClass;

	IOnlineSubsystem* Subsystem;
	//shorthand way of writting a TSharedPointer
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FName SessionJoined; //name of the current session this player is in

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result); 
	void OnFindSessionComplete(bool bSuccess); //when searching and found sessions
	void OnCreateSessionComplete(FName SessionName, bool bSuccess); //when hosting and joined session
	void OnDestroySessionComplete(FName SessionName, bool bSuccess); //when deleted session

	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
};

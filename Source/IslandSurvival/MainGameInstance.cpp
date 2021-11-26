// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MainGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "MainPlayerState.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "TimerManager.h"

UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitilize)
{
	//find the different widgets and assign them appropriatly
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetObject(TEXT("/Game/Widgets/MainMenuWidget"));
	MainMenuWidgetClass = MainMenuWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable

	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetObject(TEXT("/Game/Widgets/LobbyWidget"));
	LobbyWidgetClass = LobbyWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable

	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDWidgetObject(TEXT("/Game/Widgets/PlayerHUDWidget"));
	PlayerHUDClass = PlayerHUDWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable


	CurrentGameState = EGameState::LOBBY;
	bCrashed = false;
}

void UMainGameInstance::Init() //setup connection to specific online system
{
//	GEngine->OnNetworkFailure().AddUObject(this, &UMainGameInstance::HandleNetworkFailure);

	Subsystem = IOnlineSubsystem::Get(); //gets the subsystem for a service, in this case null as in editor
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface(); //assign the interface for this network on

		if (SessionInterface.IsValid())//if session exists assign the delegates for creating, finding, joining and destroying a session
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMainGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnJoinSessionComplete);
		}
	}
	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;
	Engine->OnNetworkFailure().AddUObject(this, &UMainGameInstance::NetworkCrash); //called if the network connection ever fails
}

void UMainGameInstance::LoadMenu() //called from the MainMenu's map blueprint to setup main menu widget
{
	if (MainMenuWidgetClass != nullptr)
		MainMenu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass); //spawn in a new widget

	if (MainMenu) //as exists now, add it to the viewport, setting up appropriate inputs
	{
		MainMenu->AddToViewport(); //add main menu to the viewport

		FInputModeUIOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		
		APlayerController* PlayerController;
		PlayerController = GetFirstLocalPlayerController();
		if (PlayerController) //if found a player controller, update its input mode
		{
			PlayerController->SetInputMode(InputMode); //tell current controller of game to use these input settings
			PlayerController->bShowMouseCursor = true; //don't hide cursor on mouse down
		}
		if (bCrashed) //if was in a session and it crashed
		{
			MainMenu->ShowErrorMenu(); //show error message
		}
	}
}

void UMainGameInstance::LoadLobby(APawn* Player) //when joinning the lobby setup the lobbies widget
{

	if (Player && Player->IsLocallyControlled()) //only make a new widget if the player is local
	{
		if (LobbyWidgetClass != nullptr)
			Lobby = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass); //spawn in a new widget

		if (Lobby) //as exists now, add it to the viewport
		{
			Lobby->AddToViewport();
			Lobby->SetVisibility(ESlateVisibility::Hidden); //hide widget

			FInputModeGameOnly InputMode; //update input mode to only accept controllers

			APlayerController* PlayerController;
			PlayerController = Cast<APlayerController>(Player->GetController());
			if (PlayerController)
			{
				PlayerController->SetInputMode(InputMode); //tell current controller of game to use these input settings
				PlayerController->bShowMouseCursor = false; //don't hide cursor on mouse down

				Lobby->SetEditability(Player); //update terrain settings editability
			}
		}
	}
}

void UMainGameInstance::PlayerDeathStart(APlayerCharacter* PlayerActor) //called when player dies to show the appropriate death message
{
	FInputModeGameAndUI InputMode; //update input mode so cannot move player

	APlayerController* PlayerController;
	PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->SetInputMode(InputMode);
		PlayerActor->DisableInput(PlayerController); //stop player from recieving any input
	}

	if (CurrentGameState == EGameState::LOBBY)
	{
		if (Lobby)
		{
			Lobby->DeathMessage->SetVisibility(ESlateVisibility::Visible);
			Lobby->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (CurrentGameState == EGameState::GAME)
	{
		if(CurrentPlayerHUDWidget)
			CurrentPlayerHUDWidget->DeathMessage->SetVisibility(ESlateVisibility::Visible);
	}
	//set timer to deactivate widget after certain time
	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &UMainGameInstance::PlayerDeathEnd, PlayerActor);

	float RespawnWaitTime = 3;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimer, RespawnDelegate, RespawnWaitTime, false); //in 2 seconds hide cancel message
}
void UMainGameInstance::PlayerDeathEnd(APlayerCharacter* PlayerActor)
{
	//update the players location and visibility
	PlayerActor->RespawnServer();

	FInputModeGameOnly InputMode; //update input mode so can move player

	APlayerController* PlayerController;
	PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->SetInputMode(InputMode);
		PlayerActor->EnableInput(PlayerController); //allow player to recieve input again
	}
	if (CurrentGameState == EGameState::LOBBY)
	{
		if (Lobby)
		{
			Lobby->DeathMessage->SetVisibility(ESlateVisibility::Collapsed);
			Lobby->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else if (CurrentGameState == EGameState::GAME)
	{
		if (CurrentPlayerHUDWidget)
			CurrentPlayerHUDWidget->DeathMessage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainGameInstance::HostSession() //make a new listen server, on client hosting game
{
	SessionJoined = TEXT("PLayerChoosenName"); //assign a name for the session
	if (Subsystem && SessionInterface.IsValid()) //if actually connected to the online system
	{
		FOnlineSessionSettings SessionSettings; //define the settings for a session
		SessionSettings.bIsLANMatch = true; //only joinable on LAN networks
		SessionSettings.NumPublicConnections = 12; //only allow max 12 people in
		SessionSettings.bShouldAdvertise = true; //allows session to be public and joinable

		SessionInterface->CreateSession(0, SessionJoined, SessionSettings); //make a new session for player on this device, with these settings
	}
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		//session sucessful so move server to the new map
		APlayerController* PlayerController;
		PlayerController = GetPrimaryPlayerController(); //get the main and only controller on machine
		if (PlayerController) //travel the player to a different map, while keeping the server active
			GetWorld()->ServerTravel(TEXT("/Game/Maps/ServerLobby?listen")); //make a new server, but still allow it to listen so others can join it
	}
}

void UMainGameInstance::JoinSession() //look up sessions to find one looking for, and then print list of them to UI
{
	SessionSearch = MakeShareable<FOnlineSessionSearch>(new FOnlineSessionSearch());
	if (Subsystem && SessionSearch.IsValid() && SessionInterface.IsValid()) //if still connected to the online system
	{
		if (SessionInterface->GetNumSessions() > 0) //if a session does exist, when leaving it, it must have failed to be destoryed
			SessionInterface->DestroySession(SessionJoined); //destroy the session on the local client

		SessionSearch->bIsLanQuery = true; //only look at sessions on same LAN
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()); //find all sessions which currently exist
	}
}

void UMainGameInstance::OnFindSessionComplete(bool bSuccess) //here actually print all sessions avaliable to a list
{
	if (bSuccess && SessionSearch.IsValid())
	{
		if (MainMenu)
			MainMenu->UpdateJoiningText("Joining Nearest Session");
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults; //get all the results from the search

		FOnlineSessionSearchResult Result; //the session actually going tp join
		for (const FOnlineSessionSearchResult& SearchResult : SearchResults) //loop through all possible sessions setting name to last one found
		{
			//update session name and result
			SessionJoined = FName(*SearchResult.GetSessionIdStr());
			Result = SearchResult;
		}
		if (SessionInterface.IsValid() && Subsystem && Result.IsValid())
		{
			SessionInterface->JoinSession(0, SessionJoined, Result); //if all valid, actually join the session
		}
		else //update UI as failed to join the session
		{
			if (MainMenu)
				MainMenu->UpdateJoiningText("Error, No Sessions Found");
		}
	}
	else  //update the UI as failed, no sessions found
	{
		if (MainMenu)
			MainMenu->UpdateJoiningText("Error, No Sessions Found");
	}
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if ((Result == EOnJoinSessionCompleteResult::Success || Result == EOnJoinSessionCompleteResult::AlreadyInSession) && SessionInterface.IsValid()) //if succesfully found the session to join
	{
		APlayerController* PlayerController;
		FString Address;
		SessionInterface->GetResolvedConnectString(SessionName, Address); //get platform specific info for joining match
		PlayerController = GetPrimaryPlayerController();
		if (PlayerController) //travel the player to a different map, while keeping the server active
		{
			CurrentGameState = EGameState::LOBBY; //update local game state enum to being in the lobby
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute); //travel to the joined session
		}
		else
		{
			if (MainMenu)
				MainMenu->UpdateJoiningText("Error, Unable to Join Session");
		}
	}
	else if(Result == EOnJoinSessionCompleteResult::AlreadyInSession)//update the UI as failed finding session, none found
	{
		if (MainMenu)
			MainMenu->UpdateJoiningText("Error, Unable to Join Session");
	}
}

void UMainGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
}

void UMainGameInstance::StartGame() //call on server only when in lobby and Start Game button pressed
{
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Terrain?listen")); //move all connected clients to the terrain map, keeping server active
}

void UMainGameInstance::LoadGame() //called on all players when loading the Terrain map
{
	CurrentGameState = EGameState::GAME; //Update Players state
	APlayerController* PlayerController;
	PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		FInputModeUIOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(InputMode);
	}

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld()); //delete any current widgets on the screen

	if (PlayerHUDClass != nullptr)
		CurrentPlayerHUDWidget = CreateWidget<UPlayerGameHUD>(GetWorld(), PlayerHUDClass); //spawn in a new widget
	if (CurrentPlayerHUDWidget)
		CurrentPlayerHUDWidget->AddToViewport(); //add players in game UI to the screen
}
void UMainGameInstance::TerrainToServer() //as Main Game State now initilized give the Game Mode the local values saved by the host
{
	AMainGameMode* MainGame = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld())); //get the game mode for this map
	if (MainGame)
	{
		MainGame->UpdateTerrainValues(Seed, TerrainWidth, TerrainHeight, bSmoothTerrain); //pass these values to the Game Mode
	}
}

void UMainGameInstance::FinishTerrainLoading() //called on each player once their terrain has finished loading in
{
	APlayerController* PlayerController;
	PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		FInputModeGameOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(InputMode);
	}
	if (CurrentPlayerHUDWidget) //will 100% fail if finished before fully loaded in terrain, unless actually already setup beforehand
		CurrentPlayerHUDWidget->HideLoading();
}


void UMainGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	QuitLobby(); //if network fails for any reason, such as host closes server, disconnect the player
}

void UMainGameInstance::QuitLobby()
{
	CurrentGameState = EGameState::LOBBY; //update game state back to being in the lobby
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	LoadMenu(); //as in main menu load it up
	APlayerController* PlayerController;
	PlayerController = GetPrimaryPlayerController(); //for this machine get primary, likly only player controller
	if (PlayerController) //travel the player to a different map, while keeping the server active
	{
		UGameplayStatics::OpenLevel(GetWorld(), "MainMenu"); //go to the MainMenu map
		SessionInterface->DestroySession(SessionJoined); //on client side delete any reference stored for session just left
	}
}

void UMainGameInstance::CancelFindSession() //if searching for sessions, cancel when button pressed
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->CancelFindSessions();
	}
}

void UMainGameInstance::NetworkCrash(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailtureType, const FString& ErrorString)
{
	QuitLobby(); //if network fails for any reason, such as host closes server, disconnect the player
	bCrashed = true;
}
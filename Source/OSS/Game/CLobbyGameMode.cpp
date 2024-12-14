#include "CLobbyGameMode.h"
#include "CGameInstance.h"

void ACLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++NumberOfPlayers;
	UE_LOG(LogTemp, Warning, TEXT("Current Players : %d"), NumberOfPlayers);

	if (NumberOfPlayers >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reached max players"));

		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ACLobbyGameMode::StartGame, 10.f);
	}
}

void ACLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--NumberOfPlayers;
}

void ACLobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UCGameInstance>(GetGameInstance());
	if (!GameInstance) return;
	GameInstance->StartSession();

	UWorld* World = GetWorld();
	if (World)
	{
		bUseSeamlessTravel = true;
		World->ServerTravel("/Game/Maps/Coop?listen");
	}
}

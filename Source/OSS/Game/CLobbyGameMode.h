#pragma once

#include "CoreMinimal.h"
#include "SessionGameMode.h"
#include "CLobbyGameMode.generated.h"

UCLASS()
class OSS_API ACLobbyGameMode : public ASessionGameMode
{
	GENERATED_BODY()
	
public:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting) override;

private:
	void StartGame();

private:
	uint32 NumberOfPlayers;

	FTimerHandle GameStartTimer;
};

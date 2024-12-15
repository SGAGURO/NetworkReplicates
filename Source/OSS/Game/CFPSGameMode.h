#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CFPSGameMode.generated.h"

class APlayerStart;

UCLASS(minimalapi)
class ACFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACFPSGameMode();

protected:
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	TArray<APlayerStart*> RedTeamPlayerStarts;
	TArray<APlayerStart*> BlueTeamPlayerStarts;

	TArray<APawn*> RedTeamPawns;
	TArray<APawn*> BlueTeamPawns;
};




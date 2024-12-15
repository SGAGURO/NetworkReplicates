#include "CFPSGameMode.h"
#include "CHUD.h"
#include "Characters/FPSCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "CPlayerState.h"
#include "OSS.h"

ACFPSGameMode::ACFPSGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Player/BP_FPSCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ACHUD::StaticClass();
	PlayerStateClass = ACPlayerState::StaticClass();
}

void ACFPSGameMode::StartPlay()
{
	Super::StartPlay();

	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
	{
		if (It->PlayerStartTag == "Red")
		{
			if (RedTeamPlayerStarts.Find(*It) < 0)
			{
				RedTeamPlayerStarts.Add(*It);
			}
		}
		else
		{
			if (BlueTeamPlayerStarts.Find(*It) < 0)
			{
				BlueTeamPlayerStarts.Add(*It);
			}
		}
	}

	LogOnScreen(this, "RedTeam : " + FString::FromInt(RedTeamPlayerStarts.Num()), FColor::Red);
	LogOnScreen(this, "BlueTeam : " + FString::FromInt(BlueTeamPlayerStarts.Num()), FColor::Blue);
}

void ACFPSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFPSCharacter* PlayerCharacter = NewPlayer->GetPawn<AFPSCharacter>();
	ACPlayerState* PS = NewPlayer->GetPlayerState<ACPlayerState>();
	if (PlayerCharacter && PS)
	{
		if (RedTeamPawns.Num() > BlueTeamPawns.Num())
		{
			PS->Team = ETeamType::Blue;
			BlueTeamPawns.Add(PlayerCharacter);
		}
		else
		{
			PS->Team = ETeamType::Red;
			RedTeamPawns.Add(PlayerCharacter);
		}
	}

	PlayerCharacter->SetTeamColor(PS->Team);
}

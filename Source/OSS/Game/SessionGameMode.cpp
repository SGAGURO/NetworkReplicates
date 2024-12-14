#include "SessionGameMode.h"
#include "Characters/SessionCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASessionGameMode::ASessionGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Player/BP_SessionCharacter"));
	if (PlayerPawnBPClass.Succeeded())
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

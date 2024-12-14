#include "CFPSGameMode.h"
#include "CHUD.h"
#include "Characters/FPSCharacter.h"

ACFPSGameMode::ACFPSGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Player/BP_FPSCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	HUDClass = ACHUD::StaticClass();
}

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CGameplayHUD.generated.h"

UCLASS()
class OSS_API UCGameplayHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDead();
};

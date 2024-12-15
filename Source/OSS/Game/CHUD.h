#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CHUD.generated.h"

class UCGameplayHUD;

UCLASS()
class ACHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACHUD();

	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;

public:
	void OnPlayerDead();

private:
	class UTexture2D* CrosshairTex;

private:
	TSubclassOf<UCGameplayHUD> GameplayHUDWidgetClass;
	UCGameplayHUD* GameplayHUDWidget;
};


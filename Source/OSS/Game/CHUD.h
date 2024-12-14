#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CHUD.generated.h"

UCLASS()
class ACHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACHUD();

	virtual void DrawHUD() override;

private:
	class UTexture2D* CrosshairTex;

};


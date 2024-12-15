#include "CHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "UI/CGameplayHUD.h"

ACHUD::ACHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPersonPack/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	static ConstructorHelpers::FClassFinder<UCGameplayHUD> WidgetClass(TEXT("/Game/UI/WB_GameplayHUD"));
	if (WidgetClass.Succeeded())
	{
		GameplayHUDWidgetClass = WidgetClass.Class;
	}
}

void ACHUD::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(GameplayHUDWidgetClass))
	{
		GameplayHUDWidget = CreateWidget<UCGameplayHUD>(GetWorld(), GameplayHUDWidgetClass);
		GameplayHUDWidget->AddToViewport();
	}
}

void ACHUD::DrawHUD()
{
	Super::DrawHUD();

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void ACHUD::OnPlayerDead()
{
	if (GameplayHUDWidget)
	{
		GameplayHUDWidget->OnPlayerDead();
	}
}
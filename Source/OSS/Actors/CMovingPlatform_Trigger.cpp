#include "CMovingPlatform_Trigger.h"
#include "Components/BoxComponent.h"
#include "CMovingPlatform.h"

ACMovingPlatform_Trigger::ACMovingPlatform_Trigger()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("RootComp");
	RootComponent = RootComp;

	BoxComp = CreateDefaultSubobject<UBoxComponent>("BoxComp");
	BoxComp->SetupAttachment(RootComp);

	BoxComp->bHiddenInGame = false;
}

void ACMovingPlatform_Trigger::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ACMovingPlatform_Trigger::OnBoxBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACMovingPlatform_Trigger::OnBoxEndOverlap);
}

void ACMovingPlatform_Trigger::OnBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	for (const auto& Platform : PlatformsToTrigger)
	{
		Platform->IncreaseActiveCount();
	}
}

void ACMovingPlatform_Trigger::OnBoxEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	for (const auto& Platform : PlatformsToTrigger)
	{
		Platform->DecreaseActiveCount();
	}
}
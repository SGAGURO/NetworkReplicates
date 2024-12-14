#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CMovingPlatform_Trigger.generated.h"

class UBoxComponent;
class ACMovingPlatform;

UCLASS()
class OSS_API ACMovingPlatform_Trigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ACMovingPlatform_Trigger();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnBoxEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UBoxComponent* BoxComp;

	UPROPERTY(EditInstanceOnly, Category = "PlatformsToTrigger")
	TArray<ACMovingPlatform*> PlatformsToTrigger;
};

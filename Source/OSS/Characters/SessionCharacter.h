#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SessionCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS(config=Game)
class ASessionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASessionCharacter();

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SprignArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;


};


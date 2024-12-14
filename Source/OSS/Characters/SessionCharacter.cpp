#include "SessionCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

ASessionCharacter::ASessionCharacter()
{
	//Collision Settings
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	//Camera Settings
	SprignArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprignArmComp"));
	SprignArmComp->SetupAttachment(RootComponent);
	SprignArmComp->TargetArmLength = 300.0f;
	SprignArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SprignArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Movement Settings
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->FallingLateralFriction = 1000.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 2000.f;
}

void ASessionCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASessionCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASessionCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASessionCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASessionCharacter::LookUpAtRate);
}

void ASessionCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASessionCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASessionCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASessionCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

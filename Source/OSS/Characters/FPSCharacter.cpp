#include "FPSCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"

AFPSCharacter::AFPSCharacter()
{
	//-------------------------------------------------------------------------
	//Properties
	//-------------------------------------------------------------------------
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	WeaponRange = 5000.f;
	WeaponDamage = 20.f;

	//-------------------------------------------------------------------------
	//CameraComp
	//-------------------------------------------------------------------------
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComp->SetupAttachment(GetCapsuleComponent());
	CameraComp->SetRelativeLocation(FVector(0, 0, 64.f));
	CameraComp->bUsePawnControlRotation = true;
	
	//-------------------------------------------------------------------------
	//FirstPerson `Arm` Mesh(Only Owner See)
	// @ Self See
	//-------------------------------------------------------------------------
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(CameraComp);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;
	FP_Mesh->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	FP_Mesh->SetRelativeRotation(FRotator(1.9f, -19.2f, 5.2f));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmMeshAsset(TEXT("/Game/FirstPersonPack/Arms/Character/Mesh/SK_Mannequin_Arms"));
	if (ArmMeshAsset.Succeeded())
	{
		FP_Mesh->SetSkeletalMesh(ArmMeshAsset.Object);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> ArmAnimClass(TEXT("/Game/FirstPersonPack/Arms/Animations/FirstPerson_AnimBP"));
	if (ArmAnimClass.Succeeded())
	{
		FP_Mesh->SetAnimInstanceClass(ArmAnimClass.Class);
	}
	
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(FP_Mesh, TEXT("GripPoint"));

	//-------------------------------------------------------------------------
	//ThirdPerson `Mannequin` Mesh(Owner No See)
	// @ Other See
	//-------------------------------------------------------------------------
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetOwnerNoSee(true);
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetOwnerNoSee(true);
	TP_Gun->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TPMeshAsset(TEXT("/Game/AnimStarterPack/UE4_Mannequin/Mesh/SK_Mannequin"));
	if (TPMeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TPMeshAsset.Object);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> TPAnimClass(TEXT("/Game/AnimStarterPack/UE4ASP_HeroTPP_AnimBlueprint"));
	if (TPAnimClass.Succeeded())
	{
		GetMesh()->SetAnimClass(TPAnimClass.Class);
	}

	//-------------------------------------------------------------------------
	//Gun Asset
	//-------------------------------------------------------------------------
	ConstructorHelpers::FObjectFinder<USkeletalMesh> GunAsset(TEXT("/Game/FirstPersonPack/FPWeapon/Mesh/SK_FPGun"));
	if (GunAsset.Succeeded())
	{
		TP_Gun->SetSkeletalMesh(GunAsset.Object);
		FP_Gun->SetSkeletalMesh(GunAsset.Object);
	}

	FP_GunshotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FP_GunshotParticle"));
	FP_GunshotParticle->bAutoActivate = false;
	FP_GunshotParticle->SetupAttachment(FP_Gun, "Muzzle");
	FP_GunshotParticle->SetOnlyOwnerSee(true);

	TP_GunshotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TP_GunshotParticle"));
	TP_GunshotParticle->bAutoActivate = false;
	TP_GunshotParticle->SetupAttachment(TP_Gun, "Muzzle");
	TP_GunshotParticle->SetOwnerNoSee(true);
}

void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::OnFire);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AFPSCharacter::ToggleCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFPSCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFPSCharacter::LookUpAtRate);
}

void AFPSCharacter::ToggleCrouch()
{
	ServerToggleCrouch();
}

void AFPSCharacter::ServerToggleCrouch_Implementation()
{
	bCrouch = !bCrouch;

	if (bCrouch)
	{
		CameraComp->SetRelativeLocation(FVector::ZeroVector);
		GetCharacterMovement()->MaxWalkSpeed = 270.f;
	}
	else
	{
		CameraComp->SetRelativeLocation(FVector(0, 0, 64));
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void AFPSCharacter::OnFire()
{
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (FP_FireAnimation != nullptr)
	{
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FP_FireAnimation, 1.f);
		}
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	FVector ShootDir = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;

	if (PlayerController)
	{
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(StartTrace, CamRot);
		ShootDir = CamRot.Vector();

		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}

	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;
	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

	AActor* DamagedActor = Impact.GetActor();
	UPrimitiveComponent* DamagedComponent = Impact.GetComponent();

	if ((DamagedActor != nullptr) && (DamagedActor != this) && (DamagedComponent != nullptr) && DamagedComponent->IsSimulatingPhysics())
	{
		DamagedComponent->AddImpulseAtLocation(ShootDir * WeaponDamage, Impact.Location);
	}
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFPSCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

FHitResult AFPSCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);

	return Hit;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, bCrouch);
}
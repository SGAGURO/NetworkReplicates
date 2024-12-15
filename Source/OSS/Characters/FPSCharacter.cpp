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
#include "Actors/CBullet.h"

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

	Health = 100.f;

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

	CrouchMovement();
}


void AFPSCharacter::OpRep_bCrouch()
{
	CrouchMovement();
}

void AFPSCharacter::CrouchMovement()
{
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

float AFPSCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageValue = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser == this)
	{
		return 0.f;
	}

	if (Health <= 0)
	{
		return 0.f;
	}

	APawn* CauserPawn = Cast<APawn>(DamageCauser);
	if (CauserPawn)
	{
		//Hitted
		Health -= Damage;

		//Dead
		if (Health <= 0)
		{
			//Todo. Self PlayerState::Death++
			//Todo. Other PlayerState::Score++
			//Todo. Resapwn via GameMode
			//Todo. Ragdoll & Cosmetic(TP, FP Mesh, Dead WidgetAnim)
			//Todo. LifeSpan
		}
	}

	return DamageValue;
}

void AFPSCharacter::OnFire()
{
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

	if (FP_GunshotParticle)
	{
		FP_GunshotParticle->ResetParticles();
		FP_GunshotParticle->SetActive(true);
	}

	ServerFire(StartTrace, EndTrace);
}

void AFPSCharacter::ServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd)
{
	WeaponTrace(LineStart, LineEnd);
	NetMulticastFire();

	if (ensure(BulletClass))
	{
		FActorSpawnParameters SpawnParam;
		SpawnParam.Instigator = this;
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(BulletClass, FP_Gun->GetSocketLocation("Muzzle"), GetControlRotation(), SpawnParam);
	}
}

void AFPSCharacter::NetMulticastFire_Implementation()
{
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (TP_GunshotParticle)
	{
		TP_GunshotParticle->ResetParticles();
		TP_GunshotParticle->SetActive(true);
	}

	if (TP_FireAnimation)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(TP_FireAnimation);
		}
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

FHitResult AFPSCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_GameTraceChannel1, TraceParams);

	if (!Hit.bBlockingHit)
	{
		return Hit;
	}

	AFPSCharacter* OtherCharacter = Cast<AFPSCharacter>(Hit.GetActor());
	if (OtherCharacter)
	{
		ACPlayerState* SelfPS = GetPlayerState<ACPlayerState>();
		ACPlayerState* OtherPS = OtherCharacter->GetPlayerState<ACPlayerState>();

		if (SelfPS && OtherPS && OtherPS->IsHostileTeam(SelfPS))
		{
			OtherCharacter->TakeDamage(WeaponDamage, FDamageEvent(), GetController(), this);
		}
	}

	return Hit;
}

void AFPSCharacter::SetTeamColor(ETeamType InTeam)
{
	BodyColor = FVector::OneVector;

	switch (InTeam)
	{
		case ETeamType::Red:
		{
			BodyColor = FVector(1, 0, 0);
		}
		break;

		case ETeamType::Blue:
		{
			BodyColor = FVector(0, 0, 1);
		}
		break;
	}

	FP_Mesh->SetVectorParameterValueOnMaterials("BodyColor", BodyColor);
	GetMesh()->SetVectorParameterValueOnMaterials("BodyColor", BodyColor);
}

void AFPSCharacter::OnRep_BodyColor()
{
	FP_Mesh->SetVectorParameterValueOnMaterials("BodyColor", BodyColor);
	GetMesh()->SetVectorParameterValueOnMaterials("BodyColor", BodyColor);
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSCharacter, bCrouch);
	DOREPLIFETIME(AFPSCharacter, Health);
	DOREPLIFETIME(AFPSCharacter, BodyColor);
}
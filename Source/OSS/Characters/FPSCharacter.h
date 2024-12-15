#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;
class ACBullet;

UCLASS(config=Game)
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* TP_Gun;

	UPROPERTY(VisibleDefaultsOnly, Category = Muzzle)
	UParticleSystemComponent* FP_GunshotParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = Muzzle)
	UParticleSystemComponent* TP_GunshotParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

public:
	AFPSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<ACBullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FP_FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* TP_FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WeaponRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float WeaponDamage;

protected:
	void MoveForward(float Val);
	void MoveRight(float Val);
		
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
		
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	
	//Fire
protected:
	void OnFire();

private:
	UFUNCTION(Reliable, Server)
	void ServerFire(const FVector& LineStart, const FVector& LineEnd);

	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastFire();

	//Crouch
protected:
	void ToggleCrouch();

	UFUNCTION(Server, Unreliable)
	void ServerToggleCrouch();

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCrouch() const { return bCrouch; }

private:
	UPROPERTY(ReplicatedUsing = "OpRep_bCrouch")
	bool bCrouch;

	UFUNCTION()
	void OpRep_bCrouch();

	void CrouchMovement();
};


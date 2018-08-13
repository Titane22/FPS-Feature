// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSCharacter.generated.h"

UENUM(BlueprintType)
enum class CharacterState :uint8
{
	Combat,
	NonCombat
};

UCLASS(config=Game)
class ATPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ADSCamera;

	class USkeletalMeshComponent* Mesh = nullptr;

	class AWeapon* EquippedWeapon = nullptr;

public:
	ATPSCharacter();

	UPROPERTY(EditAnywhere, Category = SetUp)
	float RecoveryPerTime=10.f;

	UPROPERTY(EditAnywhere, Category = SetUp)
	bool bDamaged;

	UFUNCTION(BlueprintCallable, Category = Health)
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetEquippedWeapon(class AWeapon* EquippedWeaponToSet);

	UFUNCTION(BlueprintCallable, Category = Weapon)
	class AWeapon* GetEquippedWeapon() const;

	virtual void Jump() override;
protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	virtual void Tick(float delatTime) override;
	virtual void BeginPlay() override;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	virtual void CrouchPressed();

	virtual void CrouchReleased();

	virtual void SprintPressed();

	virtual void SprintReleased();

	void ActivateAiming();

	void DeactivateAiming();

	UPROPERTY(BlueprintReadOnly, Category = SetUp)
	float Health ;

	float NonCombatTime;

	float RecoveryTime ;

	float ReloadingAnimTime;

	float FiringTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SetUp)
	bool bIsFiring;

	bool bIsAiming;

	bool bIsReloading;

	bool bFirstShoot;

	void SetFiring();

	void InitFiring();

	void Reload();
	
	void SetReloading();

	void InitReloading();

	CharacterState State;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetADSCamera(UCameraComponent* CameraToSet);

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bCrouchTrue;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bSprintTrue;

	bool GetReloading() const;
};


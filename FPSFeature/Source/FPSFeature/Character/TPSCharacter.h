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

	// Binding Action functions
	virtual void CrouchPressed();
	virtual void CrouchReleased();

	virtual void SprintPressed();
	virtual void SprintReleased();

	virtual void WalkPressed();
	virtual void WalkReleased();

	void ActivateAiming();
	void DeactivateAiming();

	UPROPERTY(BlueprintReadOnly, Category = SetUp)
	float Health ;

	float NonCombatTime;

	float RecoveryTime ;

	// Based on reloading animation time
	float ReloadingAnimTime;

	// When character is on automatic firing mode, control the fire rate
	float FiringTime;

	// if it is true, character is firing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SetUp)
	bool bIsFiring;

	// if it is true, character is aiming down sight
	bool bIsAiming;

	// if it is true, character is reloading
	bool bIsReloading;

	// if it is true, character is fire without delay
	bool bFirstShoot;

	bool bIsJump;

	bool bIsWalk;

	// Set true firing state
	void SetFiring();

	// Initialize the firing state
	void InitFiring();

	void Reload();
	
	// Set true reloading state
	void SetReloading();

	// Initialize the reloading state
	void InitReloading();

	// Has the character state
	CharacterState State;

	// Count the val, when it jumped
	int32 JumpCount;

	// Initialze the count to 2 for double jump
	int32 MaxJumpCnt;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetADSCamera(UCameraComponent* CameraToSet);


	// It is need on blueprint
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouch;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsSprint;

	// if bIsCrouch and bIsSprint is true, bIsSliding is true
	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool bIsSliding;

	bool GetReloading() const;
};


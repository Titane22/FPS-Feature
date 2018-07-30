// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TP_ThirdPersonCharacter.generated.h"

UENUM(BlueprintType)
enum class CharacterState :uint8
{
	Combat,
	NonCombat
};

UCLASS(config=Game)
class ATP_ThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	class USkeletalMeshComponent* Mesh = nullptr;
public:
	ATP_ThirdPersonCharacter();

	UPROPERTY(EditAnywhere, Category = SetUp)
	float RecoveryPerTime=10.f;

	UPROPERTY(EditAnywhere, Category = SetUp)
	bool bDamaged;

	UFUNCTION(BlueprintCallable, Category = Health)
		float GetHealth() const;
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
	UPROPERTY(BlueprintReadOnly, Category = SetUp)
	float Health = 100.f;

	float NonCombatTime = 0.f;

	float RecoveryTime = 3.f;

	float tStartTime=0.f;

	CharacterState State;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};


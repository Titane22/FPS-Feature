// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Swat_Anim.generated.h"

/**
 * 
 */
UCLASS()
class FPSFEATURE_API USwat_Anim : public UAnimInstance
{
	GENERATED_BODY()
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bCrouch;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bSprint;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FRotator AimDirection;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bReloading;
};

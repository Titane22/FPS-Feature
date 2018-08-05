// Fill out your copyright notice in the Description page of Project Settings.

#include "Swat_Anim.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void USwat_Anim::NativeUpdateAnimation(float DeltaSeconds)
{
	auto PawnOwner = Cast<ATP_ThirdPersonCharacter>(TryGetPawnOwner());

	if (PawnOwner)
	{
		// Set Direction and Speed for Anim
		Direction = CalculateDirection(PawnOwner->GetVelocity(), PawnOwner->GetActorRotation());
		Speed = PawnOwner->GetVelocity().SizeSquared();

		bCrouch = PawnOwner->bCrouchTrue;
		bSprint = PawnOwner->bSprintTrue;

		//Set AimDriection
		FRotator CurrentRotator = PawnOwner->GetControlRotation();
		float SelFloat;
		if (CurrentRotator.Yaw > 180.f)
		{
			SelFloat = 350.f - CurrentRotator.Pitch;
		}
		else
		{
			SelFloat = CurrentRotator.Pitch*(-1);
		}
		AimDirection = FRotator(0.f, 0.f, SelFloat / 3);
	}

}
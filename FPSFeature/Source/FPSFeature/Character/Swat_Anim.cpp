// Fill out your copyright notice in the Description page of Project Settings.

#include "Swat_Anim.h"
#include "TPSCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void USwat_Anim::NativeUpdateAnimation(float DeltaSeconds)
{
	auto PawnOwner = Cast<ATPSCharacter>(TryGetPawnOwner());

	if (PawnOwner)
	{
		// Set Direction and Speed for Anim
		Direction = CalculateDirection(PawnOwner->GetVelocity(), PawnOwner->GetActorRotation());
		Speed = PawnOwner->GetVelocity().Size();

		// Set bool from Character's bCrouchTrue
		bCrouch = PawnOwner->bCrouchTrue;
		bSprint = PawnOwner->bSprintTrue;
		bReloading = PawnOwner->GetReloading();

		// Set AimDirection for control rotation
		FRotator CurrentAimRotation = PawnOwner->GetControlRotation();
		float SelFloat;
		if (CurrentAimRotation.Pitch > 180.f)
		{
			SelFloat = 360.f - CurrentAimRotation.Pitch;
		}
		else
		{
			SelFloat = CurrentAimRotation.Pitch*(-1);
		}

		AimDirection = FRotator(0.f, 0.f, SelFloat / 3);
	}

}
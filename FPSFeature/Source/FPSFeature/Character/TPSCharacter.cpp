// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TPSCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATPSCharacter

ATPSCharacter::ATPSCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	Mesh = AActor::FindComponentByClass<USkeletalMeshComponent>();

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(Mesh, FName(TEXT("head")));
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Initialize the variables in class
	Health = 100.f;
	NonCombatTime = 0.f;
	RecoveryTime = 3.f;
	ReloadingAnimTime = 3.3f;
	MaxJumpCnt = 2;
}

void ATPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Location, Rotation, SpawnInfo);
	if (EquippedWeapon)UE_LOG(LogTemp, Warning, TEXT("Exist!! %s"), *EquippedWeapon->GetName()) else UE_LOG(LogTemp, Warning, TEXT("Not Exist!!"))
}

void ATPSCharacter::Tick(float deltaTime)
{
	if (State == CharacterState::Combat)
	{
		if (bDamaged)
		{
			// TODO : Apply the regeneration health system with Delay
			NonCombatTime = 0.f;
			if (Health > 0)
			{
				Health -= 5.f;
			}
			else if(Health<0.f)
			{
				Health = 0.f;
			}
		}
		else
		{
			NonCombatTime += deltaTime;
		}

		if (NonCombatTime > RecoveryTime)
		{
			Health += RecoveryPerTime;
		}

		if (Health > 100.f)
		{
			Health = 100.f;
			State = CharacterState::NonCombat;
		}
	}
	else
	{
		if (bDamaged)
		{
			State = CharacterState::Combat;
		}
	}
	//UE_LOG(LogTemp,Warning,TEXT("Health : %lf "),Health)

	// Automatic fire mode
	if (bIsFiring)
	{
		if (EquippedWeapon->GetAmmo() < 1) bIsFiring = false;
		if (bFirstShoot)
		{
			bFirstShoot = false;
			EquippedWeapon->Fire();
		}
		FiringTime += deltaTime;

		if (FiringTime > EquippedWeapon->GetFireRate())
		{
			FiringTime = 0.f;
			EquippedWeapon->Fire();
		}
	}
	else
	{
		if (FiringTime != 0.f)
		{
			FiringTime = 0.f;
		}
	}

	// Reset Jumpcount for double jump
	if (!this->GetCharacterMovement()->IsFalling() && JumpCount>0)
	{
		JumpCount = 0;
	}

	if (bIsSprint)
	{
		float FwdValue = InputComponent->GetAxisValue(FName(TEXT("MoveForward")));
		if (FwdValue == 0)
		{
			bIsSprint = false;
			this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
		}
	}

}

void ATPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("Primary Fire", IE_Pressed, this, &ATPSCharacter::SetFiring);
	PlayerInputComponent->BindAction("Primary Fire", IE_Released, this, &ATPSCharacter::InitFiring);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ATPSCharacter::Reload);

	PlayerInputComponent->BindAction("AimDownSights", IE_Pressed, this, &ATPSCharacter::ActivateAiming);
	PlayerInputComponent->BindAction("AimDownSights", IE_Released, this, &ATPSCharacter::DeactivateAiming);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPSCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSCharacter::CrouchPressed);
	//PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPSCharacter::CrouchReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPSCharacter::SprintPressed);
	//PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPSCharacter::SprintReleased);

	PlayerInputComponent->BindAction("Walk", IE_Pressed, this, &ATPSCharacter::WalkPressed);
	PlayerInputComponent->BindAction("Walk", IE_Released, this, &ATPSCharacter::WalkReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPSCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	
}

void ATPSCharacter::CrouchPressed()
{
	bIsCrouch = !bIsCrouch;
	if (!bIsCrouch)
	{
		bIsCrouch = false;
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else if (bIsCrouch)
	{
		if (!bIsSprint && !bIsWalk)
		{
			bIsCrouch = true;
			this->GetCharacterMovement()->MaxWalkSpeed = 300.f;
		}
		else if (bIsSprint && !bIsWalk && !bIsFiring && !bIsAiming && !bIsReloading)
		{
			UE_LOG(LogTemp, Warning, TEXT("Called"))
			bIsSprint = false;
			bIsSliding = true;
		}
	}
}

void ATPSCharacter::CrouchReleased()
{
	if (bIsCrouch)
	{
		bIsCrouch = false;
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else if (bIsSliding)
	{
		bIsSliding = false;
	}
}

void ATPSCharacter::SprintPressed()
{
	bIsSprint = !bIsSprint;
	if (!bIsSprint)
	{
		bIsSprint = false;
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else
	{
		float FwdValue = InputComponent->GetAxisValue(FName(TEXT("MoveForward")));
		float RValue = InputComponent->GetAxisValue(FName(TEXT("MoveRight")));
		if (!bIsCrouch && FwdValue > 0.f && !bIsAiming && !bIsFiring && !bIsReloading && !bIsWalk)
		{
			bIsSprint = true;
			this->GetCharacterMovement()->MaxWalkSpeed = 1000.f;
		}
	}
}

void ATPSCharacter::SprintReleased()
{
	if (bIsSprint)
	{
		bIsSprint = false;
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ATPSCharacter::WalkPressed()
{
	if (!bIsWalk && !bIsSprint && !bIsCrouch)
	{
		bIsWalk = true;
		this->GetCharacterMovement()->MaxWalkSpeed = 300.f;
	}
}

void ATPSCharacter::WalkReleased()
{
	if (bIsWalk)
	{
		bIsWalk = false;
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ATPSCharacter::ActivateAiming()
{
	if (FollowCamera && ADSCamera)
	{
		bIsAiming = true;
		FollowCamera->Deactivate();
		ADSCamera->Activate();
		this->GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
}

void ATPSCharacter::DeactivateAiming()
{
	if (FollowCamera && ADSCamera)
	{
		bIsAiming = false;
		FollowCamera->Activate();
		ADSCamera->Deactivate();
		this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void ATPSCharacter::SetFiring()
{
	if (EquippedWeapon && !bIsReloading && !bIsSprint)
	{
		if (EquippedWeapon->GetAmmo() < 1)
		{
			bIsFiring = false;
			return;
		}
		bFirstShoot = true;
		bIsFiring = true;
	}
}

void ATPSCharacter::InitFiring()
{
	if (bIsFiring)
	{
		bIsFiring = false;
	}
}

void ATPSCharacter::Reload()
{
	if (EquippedWeapon&& EquippedWeapon->isSameClipSize())
	{
		if (bIsFiring) bIsFiring = false;
		
		SetReloading();
		
		FTimerHandle Timer;
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &ATPSCharacter::InitReloading, ReloadingAnimTime, false);
	}
}

void ATPSCharacter::SetReloading()
{
	bIsReloading = true;
}

void ATPSCharacter::InitReloading()
{
	EquippedWeapon->Reload();
	bIsReloading = false;
}

void ATPSCharacter:: SetADSCamera(UCameraComponent * CameraToSet)
{
	ADSCamera = CameraToSet;
}

bool ATPSCharacter::GetReloading() const
{
	return bIsReloading;
}

float ATPSCharacter::GetHealth() const
{
	return Health;
}

void ATPSCharacter::SetEquippedWeapon(AWeapon * EquippedWeaponToSet)
{
	EquippedWeapon = EquippedWeaponToSet;
}

AWeapon* ATPSCharacter::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

void ATPSCharacter::Jump()
{
	if (!bIsFiring)
	{
		if (JumpCount < MaxJumpCnt)
		{
			if (bIsSprint)
			{
				bIsSprint = false;
				this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
			}
			JumpCount++;
			Super::Jump();
		}
	}
}

void ATPSCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATPSCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

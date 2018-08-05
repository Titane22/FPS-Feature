// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
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

}

void ATP_ThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(Location, Rotation, SpawnInfo);
	if (EquippedWeapon)UE_LOG(LogTemp, Warning, TEXT("Exist!! %s"), *EquippedWeapon->GetName()) else UE_LOG(LogTemp, Warning, TEXT("Not Exist!!"))
	Weapon->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("Weapon_Attach")));

	InputComponent->BindAction("Fire", IE_Pressed, EquippedWeapon, &AWeapon::Fire);
}

void ATP_ThirdPersonCharacter::Tick(float deltaTime)
{
	if (State == CharacterState::Combat)
	{
		if (bDamaged)
		{
			tStartTime +=1.f;
			if (tStartTime > 11.f) tStartTime = 0.f;
			//UE_LOG(LogTemp, Warning, TEXT("tStartTime : %lf "), tStartTime)
			NonCombatTime = 0.f;
			if (Health > 0 && tStartTime>10.f)
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
		//UE_LOG(LogTemp, Warning, TEXT("Non-Combat "))
		if (bDamaged)
		{
			State = CharacterState::Combat;
		}
	}

	//UE_LOG(LogTemp,Warning,TEXT("Health : %lf "),Health)
}

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATP_ThirdPersonCharacter::CrouchPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATP_ThirdPersonCharacter::CrouchReleased);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATP_ThirdPersonCharacter::SprintPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATP_ThirdPersonCharacter::SprintReleased);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATP_ThirdPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATP_ThirdPersonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	
}

void ATP_ThirdPersonCharacter::CrouchPressed()
{
	bCrouchTrue = true;
	this->GetCharacterMovement()->MaxWalkSpeed = 300.f;
}

void ATP_ThirdPersonCharacter::CrouchReleased()
{
	bCrouchTrue = false;
	this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ATP_ThirdPersonCharacter::SprintPressed()
{
	bSprintTrue = true;
	this->GetCharacterMovement()->MaxWalkSpeed = 1000.f;
}

void ATP_ThirdPersonCharacter::SprintReleased()
{
	bSprintTrue = false;
	this->GetCharacterMovement()->MaxWalkSpeed = 600.f;
}


float ATP_ThirdPersonCharacter::GetHealth() const
{
	return Health;
}

void ATP_ThirdPersonCharacter::SetEquippedWeapon(AWeapon * EquippedWeaponToSet)
{
	EquippedWeapon = EquippedWeaponToSet;
}

void ATP_ThirdPersonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATP_ThirdPersonCharacter::MoveRight(float Value)
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



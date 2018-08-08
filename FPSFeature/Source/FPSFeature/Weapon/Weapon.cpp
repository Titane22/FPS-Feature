// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "../Character/TPSCharacter.h"
#include "Projectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh= CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;
	//Mesh->SetMaterial(0,);
	Mesh->SetupAttachment(RootComponent);
	
	ProjectileClass = AProjectile::StaticClass();
	WeaponOwner = Cast<ATPSCharacter>(this->GetOwner());
	
	// Initialize the gun
	FireRate = 0.18f;
	Ammo = 30;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AWeapon::Fire()
{
	// TODO : If the player is running, don't fire
	if (ProjectileClass)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			auto MuzzleTransform = Mesh->GetSocketTransform(FName(TEXT("Muzzle")));
			// AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(AProjectile::StaticClass(), MuzzleTransform);
			World->SpawnActor<AProjectile>(ProjectileClass, MuzzleTransform, ActorSpawnParams);
			Ammo--;
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile Class is empty"))
	}
}

float AWeapon::GetFireRate() const
{
	return FireRate;
}

int32 AWeapon::GetAmmo() const
{
	return Ammo;
}

void AWeapon::SetProejctile(AProjectile * ProjectileToSet)
{
	ProjectileBP = ProjectileToSet;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


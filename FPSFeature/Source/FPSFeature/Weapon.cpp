// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
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

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AWeapon::Fire()
{
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

		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No"))
	}
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


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FPSFEATURE_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Mesh = nullptr;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectile> ProjectileClass;

	// TODO : Add Static Mesh to Projectile class and then delete this
	UPROPERTY(EditAnywhere, Category = Projectile)
	class AProjectile* ProjectileBP = nullptr;

	class ATPSCharacter* WeaponOwner = nullptr;
public:	
	// Sets default values for this actor's properties
	AWeapon();

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, Category = Components)
		class UParticleSystemComponent* LaunchBlast = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float FireRate;
	
	int32 Ammo;

	int32 MaxAmmo;

	int32 ClipSize;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Fire();

	void Reload();

	float GetFireRate() const;

	UFUNCTION(BlueprintCallable, Category = Category)
	int32 GetAmmo() const;

	UFUNCTION(BlueprintCallable, Category = Category)
	int32 GetMaxAmmo() const;

	bool isSameClipSize();

	// TODO : Add Static Mesh to Projectile class and then delete this
	UFUNCTION(BlueprintCallable, Category = Category)
	void SetProejctile(class AProjectile* ProjectileToSet);
};

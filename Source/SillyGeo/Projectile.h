// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SILLYGEO_API AProjectile : public AActor
{
	GENERATED_BODY()
	
	/* sphere root collision component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollision;
	
	/* projectile trail emitter component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ProjectileTrail;
	
	/*  projectile light  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UPointLightComponent* ProjectileLight;
	
	/** projectile movement component  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovementComponent;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* ExplosionSound;

	/* explosion emitter template */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplosionEmitter;

protected:

	// Sets default values for this actor's properties
	AProjectile();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** calls when sphere overlaps other actor  */
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
private:

	/** calls when projectile overlaps anything   */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnExplosionFX();

	/** color to apply to projectile light,
	*	projectile trail and projectile explosion FX
	*	depends on owner current weapon (Current Color)
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor ProjectileColor = FLinearColor::Black;

	/** damage to cause to victim  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageToCause = 50.f;
	
};

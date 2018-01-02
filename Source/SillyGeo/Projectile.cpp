// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ConstructorHelpers.h"
#include "Geo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyBase.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/** sphere collision  */
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SetRootComponent(SphereCollision);
	SphereCollision->SetSphereRadius(8.f);
	SphereCollision->SetCollisionProfileName("ProjectileBase");

	/** projectile trail  */
	ProjectileTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileTrail"));
	ProjectileTrail->SetupAttachment(RootComponent);
	
	/** set emitter template  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystem(TEXT("/Game/Weapons/Particles/PFX_Param_Projectile"));
	if (ParticleSystem.Object)
	{
		ProjectileTrail->SetTemplate(ParticleSystem.Object);
	}
	
	/** projectile light  */
	ProjectileLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ProjectileLight"));
	ProjectileLight->SetupAttachment(RootComponent);
	ProjectileLight->Intensity = 7500.f;
	ProjectileLight->AttenuationRadius = 2500.f;
	ProjectileLight->CastShadows = false;
	ProjectileLight->SourceRadius = 5.f;
	
	/** projectile movement component  */
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
	ProjectileMovementComponent->Velocity = FVector(500.f, 0.f, 0.f);
	ProjectileMovementComponent->bConstrainToPlane = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	/** set explosion sound  */
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundTemplate(TEXT("/Game/Player/Audio/FlashImpact_Cue"));
	if (ExplosionSoundTemplate.Succeeded())
	{
		ExplosionSound = ExplosionSoundTemplate.Object;
	}

	/** set explosion emitter template  */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEmitterTemplate(TEXT("/Game/Weapons/Particles/PFX_Param_Explosion"));
	if (ExplosionEmitterTemplate.Succeeded())
	{
		ExplosionEmitter = ExplosionEmitterTemplate.Object;
	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnOverlapBegin);

	/** set color for trail and projectile light, add inherited velocity from owner  */
	if(AGeo* Geo = Cast<AGeo>(GetOwner()))
	{
		/** set velocity  */
		FVector OwnerVelocity = FVector(FMath::Abs(Geo->GetCharacterMovement()->Velocity.X), 0.f, 0.f);
		FVector NewVelocity = ProjectileMovementComponent->Velocity + OwnerVelocity;
		ProjectileMovementComponent->SetVelocityInLocalSpace(NewVelocity);

		/** set color for projectile trail and for projectile light */
		ProjectileColor = Geo->GetCurrentWeaponColor();
		ProjectileLight->SetLightColor(ProjectileColor);
		ProjectileTrail->SetColorParameter("ProjectileColor", ProjectileColor);
	}
}

void AProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself. 
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && !OtherActor->IsPendingKill())
	{
		/** explode whatever we hit  */
		SpawnExplosionFX();

		/** if we hit an enemy  */
		if(AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
		{
			/** and color are same */
			if (Enemy->GetEnemyColor() == ProjectileColor)
			{
				AController* InstigatorController = nullptr;
				if (GetOwner())
				{
					InstigatorController = GetOwner()->GetInstigatorController();
				}

				/** inflict damage to this enemy  */
				Enemy->TakeDamage(DamageToCause, FDamageEvent(), InstigatorController, this);
			}
		}
		
		Destroy();
	}
}

void AProjectile::SpawnExplosionFX()
{
	/** spawn explosion FX  */
	if (ExplosionEmitter)
	{
		FVector SpawnLocation = SphereCollision->GetComponentLocation();
		FRotator SpawnRotation = FRotator(0.f, 0.f, 0.f);
		UParticleSystemComponent* ExplosionFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEmitter, SpawnLocation, SpawnRotation);
		if (ExplosionFX)
		{
			ExplosionFX->SetColorParameter("BlastColor", ProjectileColor);
		}
	}

	// try and play the sound if specified
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}


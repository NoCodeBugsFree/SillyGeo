// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ConstructorHelpers.h"
#include "GeoGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Geo.h"
#include "Kismet/KismetMathLibrary.h"
#include "SillyGeoGameMode.h"
#include "GeoPlayerState.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/* hit sphere  */
	HitSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Hit Sphere"));
	SetRootComponent(HitSphere);
	HitSphere->SetSphereRadius(32.f);
	HitSphere->SetNotifyRigidBodyCollision(true);
	HitSphere->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	HitSphere->SetCollisionProfileName("EnemyBase");

	/** EnemyMesh  */
	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Enemy Mesh"));
	EnemyMesh->SetupAttachment(RootComponent);
	EnemyMesh->SetRelativeLocation(FVector(0.f, 0.f, HitSphere->GetScaledSphereRadius() / -2.f));
	EnemyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/** set mesh */
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EnemyMeshTempalte(TEXT("/Game/Shapes/Shape_Cone"));
	if (EnemyMeshTempalte.Succeeded())
	{
		EnemyMesh->SetStaticMesh(EnemyMeshTempalte.Object);
	}

	/** set mesh material */
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialTempalte(TEXT("/Game/Enemies/Materials/MI_RedEnemy_Pulse"));
	if (MaterialTempalte.Succeeded())
	{
		EnemyMesh->SetMaterial(0, MaterialTempalte.Object);
	}

	/** set emitter template */
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionEmitterTempalte(TEXT("/Game/Enemies/Particles/PFX_EnemyExplosion"));
	if (ExplosionEmitterTempalte.Succeeded())
	{
		ExplosionEmitter = ExplosionEmitterTempalte.Object;
	}

	/** set explosion sound  */
	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundTemplate(TEXT("/Game/Player/Audio/FlashImpact_Cue"));
	if (ExplosionSoundTemplate.Succeeded())
	{
		ExplosionSound = ExplosionSoundTemplate.Object;
	}

	/* enemy movement  */
	EnemyMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("EnemyMovement"));
	EnemyMovement->ProjectileGravityScale = 0.f;
	EnemyMovement->bShouldBounce = true;
	EnemyMovement->Bounciness = 1.f;
	EnemyMovement->Friction = 0.f;
	EnemyMovement->bConstrainToPlane = true;
	EnemyMovement->bSnapToPlaneAtStart = true;
	EnemyMovement->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));

	/** class defaults  */
	bSpinning = false;
	bRandomShift = false;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AEnemyBase::OnEnemyOverlapBegin);
	
	/** sets a target to follow  */
	InitTarget();

	/** starts enemies timers  */
	StartTimers();

	RandomVector = FMath::VRand();	
}

void AEnemyBase::InitTarget()
{
	/** if no player to chase - moving randomly  */
	if (!PlayerPawn)
	{
		const UWorld* World = GetWorld();
		if (World)
		{
			if (ASillyGeoGameMode* SillyGeoGameMode = Cast<ASillyGeoGameMode>(World->GetAuthGameMode()))
			{
				PlayerPawn = SillyGeoGameMode->GetRandomPlayerPawn();
				if (PlayerPawn == nullptr)
				{
					bRandomShift = true;
					UE_LOG(LogTemp, Error, TEXT("PlayerPawn == nullptr"));
				}
			}
		}
	}
}

void AEnemyBase::StartTimers()
{
	if (bRandomShift)
	{
		FTimerDelegate RandomShiftDelegate;
		RandomShiftDelegate.BindLambda([&]
		{
			RandomDirection = FMath::VRand() * FMath::RandRange(800.f, 8000.f);
		}
		);

		FTimerHandle RandomShiftTimer;
		GetWorldTimerManager().SetTimer(RandomShiftTimer, RandomShiftDelegate, FMath::RandRange(0.5f, 1.f), true);
	}

	/** start random change behavior  */
	FTimerHandle TrackTimer;
	GetWorldTimerManager().SetTimer(TrackTimer, this, &AEnemyBase::Tracking, TrackingDelay, true);
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** calls to follow the player  */
	Follow();
}

void AEnemyBase::SetDefaultValues(bool bNewSpinning /*= false*/, bool bShifting /*= false*/, EEnemyColor Color /*= EEnemyColor::EN_Red*/, class UMaterialInterface* Mat /*= nullptr*/, class UStaticMesh* Mesh /*= nullptr*/, float Speed /*= 400.f*/)
{
	bSpinning = bNewSpinning;
	bRandomShift = bShifting;
	EnemyColor = Color;
	CoreMaterial = Mat;
	EnemyMesh->SetStaticMesh(Mesh);

	switch (EnemyColor)
	{
	case EEnemyColor::EN_Blue:
		CurrentColor = FLinearColor::Blue;
		break;

	case EEnemyColor::EN_Green:
		CurrentColor = FLinearColor::Green;
		break;

	case EEnemyColor::EN_Red:
		CurrentColor = FLinearColor::Red;
		break;

	case EEnemyColor::EN_Yellow:
		CurrentColor = FLinearColor::Yellow;
		break;

	default:
		break;
	}

	/** init speed  */
	EnemyMovementSpeed = Speed;

	/** create enemy dynamic material  */
	if (EnemyMesh)
	{
		EnemyDynamicMaterial = EnemyMesh->CreateDynamicMaterialInstance(0, CoreMaterial);
		if (EnemyDynamicMaterial)
		{
			EnemyDynamicMaterial->SetVectorParameterValue("EnemyColor", CurrentColor);
		}
	}
}

void AEnemyBase::Tracking()
{
	FVector PlayerLocaion = PlayerPawn ? PlayerPawn->GetActorLocation() * FVector(1.f, 1.f, 0.f) : FVector(0.f, 0.f, 0.f);
	FVector MyLocation = GetActorLocation() + RandomDirection * FVector(1.f, 1.f, 0.f);
	FRotator RotatorFromX = FRotationMatrix::MakeFromX(PlayerLocaion - MyLocation).Rotator();
	Destination = RotatorFromX.RotateVector(FVector(EnemyMovementSpeed, 0.f, 0.f));
}

void AEnemyBase::Follow()
{
	EnemyMovement->Velocity = Destination;
}

void AEnemyBase::OnEnemyOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor && OtherActor != this && !OtherActor->IsPendingKill())
	{
		/** destroy only if we hit Geo  */
		if(AGeo* Geo = Cast<AGeo>(OtherActor))
		{
			/** damage Geo  */
			Geo->TakeDamage(DamageToCause, FDamageEvent(), nullptr, nullptr);

			/** kill self  */
			this->TakeDamage(Health, FDamageEvent(), nullptr, nullptr);

			Destroy();
		}
	}
}

float AEnemyBase::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		
		if (!IsPendingKill())
		{
			if (Health <= 0.f) /** we are dead  */
			{
				SpawnExplodeFX();
				
				/** add 1 enemy killed to player state  */
				if (EventInstigator)
				{
					if(AGeo* Geo = Cast<AGeo>(EventInstigator->GetPawn()))
					{
						if (AGeoPlayerState* GeoPlayerState = Cast<AGeoPlayerState>(Geo->PlayerState))
						{
							GeoPlayerState->EnemiesKilled++;
						}
					}
				}

				/** update game state  */
				if (GeoGameState && GeoGameMode)
				{
					/** remove one enemy  */
					GeoGameState->AddEnemiesRemaining(-1);

					GeoGameMode->UpdateHUD();

					/** if we haven't alive enemies on map and we haven't enemies to spawn */
					if (GeoGameState->GetEnemiesRemaining() <= 0 && !GeoGameMode->HasEnemiesToSpawn())
					{
						GeoGameMode->EndWave();
					}
				}
				Destroy();
			}
		}
	}

	return ActualDamage;
}

void AEnemyBase::SpawnExplodeFX()
{
	/** spawn explosion FX  */
	if (ExplosionEmitter)
	{
		UParticleSystemComponent* ExplodeFX = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEmitter, GetActorTransform());
		if (ExplodeFX)
		{
			/** set color according enemy color  */
			ExplodeFX->SetColorParameter("EnemyColor", CurrentColor);
		}
	}

	// try and play the sound if specified
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
}

void AEnemyBase::InitReferences(class ASillyGeoGameMode* NewGM, class AGeoGameState* NewGS)
{
	if (!ensure(NewGM)) { return; }
	if (!ensure(NewGS)) { return; }

	GeoGameState = NewGS;
	GeoGameMode = NewGM;
}

void AEnemyBase::SetTarget(class APawn* TargetPawn)
{
	PlayerPawn = TargetPawn;
}

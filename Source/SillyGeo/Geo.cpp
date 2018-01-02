// Fill out your copyright notice in the Description page of Project Settings.

#include "Geo.h"
#include "GeoGameState.h"
#include "Particles/ParticleSystemComponent.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GeoPlayerController.h"

// Sets default values
AGeo::AGeo()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** root capsule  */
	GetCapsuleComponent()->SetCapsuleHalfHeight(18.f);
	GetCapsuleComponent()->SetCapsuleRadius(18.f);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetCollisionProfileName("PlayerPawn");
	
	/** disable skeletal mesh gravity  */
	GetMesh()->SetEnableGravity(false);

	/** wings scene */
	Wings = CreateDefaultSubobject<USceneComponent>(TEXT("Wings"));
	Wings->SetupAttachment(RootComponent);

	/** pre rotate wings scene  */
	PreRotateWings = CreateDefaultSubobject<USceneComponent>(TEXT("PreRotateWings"));
	PreRotateWings->SetupAttachment(Wings);
	PreRotateWings->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	/** camera scene */
	CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
	CameraRoot->SetupAttachment(RootComponent);
	CameraRoot->SetRelativeLocation(FVector(0.f, 0.f, 500.f));

	/** camera  */
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraRoot);
	PlayerCamera->SetRelativeRotation(FRotator(-90.f, 0.f, -90.f));

	/** reticle scene */
	ReticleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ReticleLoc"));
	ReticleLocation->SetupAttachment(RootComponent);

	/** weapon wings  */
	WeaponWings = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponWings"));
	WeaponWings->SetupAttachment(PreRotateWings);

	/** player body */
	Puck = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Puck"));
	Puck->SetupAttachment(RootComponent);

	/** emitter trail  */
	Trail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail"));
	Trail->SetupAttachment(Puck);
	Trail->SetRelativeLocation(FVector(0.f, 0.f, -15.f));

	/** background sparks emitter  */
	Sparks = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Sparks"));
	Sparks->SetupAttachment(CameraRoot);
	Sparks->SetRelativeLocation(FVector(0.f, 0.f, -500.f));

	/** reticle mesh  */
	Reticle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reticle"));
	Reticle->SetupAttachment(ReticleLocation);
	Reticle->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	/** 2D flying movement setup  */
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->AirControl = 1.f;
	GetCharacterMovement()->FallingLateralFriction = 1.f;
	GetCharacterMovement()->MaxAcceleration = 1024.f;
	GetCharacterMovement()->DefaultLandMovementMode = EMovementMode::MOVE_Flying;
	GetCharacterMovement()->GroundFriction = 0.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 2000.f;
	GetCharacterMovement()->BrakingDecelerationSwimming = 2000.f;
	GetCharacterMovement()->NavAgentProps.AgentRadius = 18.f;
	GetCharacterMovement()->NavAgentProps.AgentHeight = 36.f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.f, 0.f, 1.f));

#pragma region Helpers

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponWingsTemplate(TEXT("/Game/Player/Meshes/SM_Wing_Simple"));
	if (WeaponWingsTemplate.Succeeded())
	{
		WeaponWings->SetStaticMesh(WeaponWingsTemplate.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PuckTemplate(TEXT("/Game/Player/Meshes/SM_PlayerCenter"));
	if (PuckTemplate.Succeeded())
	{
		Puck->SetStaticMesh(PuckTemplate.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> TrailTemplate(TEXT("/Game/Player/Particles/PFX_PlayerTrail"));
	if (TrailTemplate.Succeeded())
	{
		Trail->SetTemplate(TrailTemplate.Object);
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SparksTemplate(TEXT("/Game/Weapons/Particles/PFX_WorldSparks"));
	if (SparksTemplate.Succeeded())
	{
		Sparks->SetTemplate(SparksTemplate.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ReticleTemplate(TEXT("/Game/Player/Meshes/SM_TargetMesh"));
	if (ReticleTemplate.Succeeded())
	{
		Reticle->SetStaticMesh(ReticleTemplate.Object);
	}

	/** set projectile template class  */
	static ConstructorHelpers::FClassFinder<AProjectile> ProjectileTemplateClass(TEXT("/Game/BP/BP_Projectile"));
	if (ProjectileTemplateClass.Class)
	{
		ProjectileTemplate = ProjectileTemplateClass.Class;
	}

	/** set fire sound  */
	static ConstructorHelpers::FObjectFinder<USoundBase> FireSoundTemplate(TEXT("/Game/Player/Audio/SimpleWeaponShot_Cue"));
	if (FireSoundTemplate.Succeeded())
	{
		FireSound = FireSoundTemplate.Object;
	}

	/** set camera curve  */
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CameraCurveTemplate(TEXT("/Game/Player/Blueprint/CamCurve"));
	if (CameraCurveTemplate.Succeeded())
	{
		CameraCurve = CameraCurveTemplate.Object;
	}

#pragma endregion


	/** Geo defaults  */
	bCanFire			= true;
	bLeftMuzzle			= true;
	bCanSwitchWeapon	= true;
}

void AGeo::OnConstruction(const FTransform& Transform)
{
	/** set default colors   */
	WeaponColors.Empty();
	WeaponColors.Add(FLinearColor::Red);
	WeaponColors.Add(FLinearColor::Blue);
	WeaponColors.Add(FLinearColor::Green);
	WeaponColors.Add(FLinearColor::Yellow);
	
	
	/** override colors in BP if needed  */
	Super::OnConstruction(Transform);

	/** set the wings color according to current weapon number and weapon color  */
	SetWingsAndTrailColor();
	
	/** create a dynamic material for reticle  */
	ReticleDynamicMaterial = Reticle->CreateDynamicMaterialInstance(0);
}

void AGeo::SetWingsAndTrailColor()
{
	WingsDynamicMaterial = WeaponWings->CreateDynamicMaterialInstance(0);
	if (WingsDynamicMaterial)
	{
		/** if something will wrong - use default gray color  */
		FLinearColor ColorToSet = FLinearColor::Gray;
		if (WeaponColors.IsValidIndex(CurrentWeapon))
		{
			ColorToSet = WeaponColors[CurrentWeapon];
		}

		/** set wings color  */
		WingsDynamicMaterial->SetVectorParameterValue("WingColor", ColorToSet);

		/** set emitter trail color  */
		Trail->SetColorParameter("TrailColor", ColorToSet);
	}
}

// Called when the game starts or when spawned
void AGeo::BeginPlay()
{
	Super::BeginPlay();

	/** check wing list   */
	if (!ensure(WingList.Num() != 0)) { return; }

	/** setup player controller  */
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PlayerController = PC;
		PlayerController->bShowMouseCursor = true;
		PlayerController->CurrentMouseCursor = EMouseCursor::Crosshairs;
	}
	if (!ensure(PlayerController)) { return; }
	
	InitializePlayer();
}

// Called every frame
void AGeo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	/** calls to rotate weapon towards mouse cursor  */
	RotateToMouseCursor(DeltaTime);

	/** calls to zoom in/out camera from player according to players speed */
	ZoomCamera();
	
}

// Called to bind functionality to input
void AGeo::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement
	InputComponent->BindAxis("MovementX", this, &AGeo::MovementX);
	InputComponent->BindAxis("MovementY", this, &AGeo::MovementY);

	// fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AGeo::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AGeo::StopFire);

	// switch weapon
	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &AGeo::NextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Released, this, &AGeo::PreviousWeapon);
}

float AGeo::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		
		// we are dead
		if (Health <= 0.f)
		{
			/** disable Geo  */
			SetActorEnableCollision(false);
			SetActorHiddenInGame(true);
			SetActorTickEnabled(false);
			if(AGeoPlayerController* GeoPC = Cast<AGeoPlayerController>(Controller))
			{
				DisableInput(GeoPC);
				GeoPC->LoseTheGame();
				GeoPC->StartSpectatingOnly();
				GeoPC->bShowMouseCursor = false;
			}

			/** stop firing  */
			GetWorldTimerManager().ClearTimer(FireTimer);

			/** stop regeneration  */
			GetWorldTimerManager().ClearTimer(RegenTimer);

			/** TODO GameMode-> Reset TARGET  for all enemies */
		}
	}

	return ActualDamage;
}

void AGeo::NextWeapon()
{
	if (bCanSwitchWeapon)
	{
		bCanSwitchWeapon = false;
		SwitchWeapon(1);
	}
}

void AGeo::PreviousWeapon()
{
	if (bCanSwitchWeapon)
	{
		bCanSwitchWeapon = false;
		SwitchWeapon(-1);
	}
}

void AGeo::SwitchWeapon(int32 WeaponStep)
{
	if (!ensure(WeaponColors.Num() != 0)) { return; }

	/** save the previous weapon color to lerp from  */
	if (WeaponColors.IsValidIndex(CurrentWeapon))
	{
		PreviousWeaponColor = WeaponColors[CurrentWeapon];
	}
	
	if (WeaponStep > 0)
	{
		CurrentWeapon++;

		/** if we trying go out of array bounds - we must go to 1st ( zero ) index  */
		if (!WeaponColors.IsValidIndex(CurrentWeapon)) 
		{
			CurrentWeapon = 0;
		}
	} 
	else
	{
		CurrentWeapon--;
		/** if we trying go out of array bounds - we must go to last index  */
		if (!WeaponColors.IsValidIndex(CurrentWeapon))
		{
			CurrentWeapon = WeaponColors.Num() - 1;
		}
	}

	/** BP timeline weapon switch FX  */
	WeaponSwitchFX();
	
	/** update wings and trail color according to current weapon */
	SetWingsAndTrailColor();
}

void AGeo::InitializePlayer()
{
	/** set player controller reference  */
	if (APlayerController* TestPC = Cast<APlayerController>(Controller))
	{
		PC = TestPC;
	}
	if (!ensure(PC)) { return; }

	/** set game state reference  */
	if (AGeoGameState* TestGeoGameState = Cast<AGeoGameState>(GetWorld()->GetGameState()))
	{
		GeoGameState = TestGeoGameState;
	}
	if (!ensure(GeoGameState)) { return; }

	/** reset health  */
	Health = MaxHealth;
	EnableInput(PC);
	PC->EnableInput(PC);
		
	/** start regeneration timer */
	GetWorldTimerManager().SetTimer(RegenTimer, this, &AGeo::RegenerateHealth, RegenerationDelay, true);
}

void AGeo::RegenerateHealth()
{
	if (GeoGameState)
	{
		if (Health < MaxHealth)
		{
			Health = FMath::Clamp(Health + 2.f, 0.f, MaxHealth);
		}
	}
}

void AGeo::StartFire()
{
	if (bCanFire)
	{
		GetWorldTimerManager().SetTimer(FireTimer, this, &AGeo::Fire, FireRate, true, 0.f);
	}
}

void AGeo::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireTimer);
}

void AGeo::Fire()
{
	if (ProjectileTemplate)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			
			FTransform SpawnTransform = bLeftMuzzle ? WeaponWings->GetSocketTransform("Weapon_A") : WeaponWings->GetSocketTransform("Weapon_B");

			
			AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileTemplate, SpawnTransform, SpawnParams);
			if (SpawnedProjectile)
			{
				/** cooldown  */
				bCanFire = false;
				FTimerDelegate CooldownDelegate;
				CooldownDelegate.BindLambda([&] {bCanFire = true;  });
				FTimerHandle CoolDownTimer;
				GetWorldTimerManager().SetTimer(CoolDownTimer, CooldownDelegate, FireRate, false);


				/** change muzzle for next shot  */
				bLeftMuzzle = !bLeftMuzzle;
				
				// try and play the sound if specified
				if (FireSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProjectileTemplate == NULL"));
	}
}

void AGeo::MovementX(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(FVector(Value, 0.f, 0.f));
	}
}

void AGeo::MovementY(float Value)
{
	if (Value != 0.f)
	{
		AddMovementInput(FVector(0.f, -Value, 0.f));
	}
}

void AGeo::ZoomCamera()
{
	if (CameraCurve)
	{
		float Alpha = CameraCurve->GetFloatValue(GetCharacterMovement()->Velocity.Size() / GetCharacterMovement()->MaxFlySpeed);
		PlayerCamera->SetRelativeLocation(FVector(0.f, 0.f, FMath::Lerp(0.f, CameraZoomMax, Alpha)));
	}
}

void AGeo::RotateToMouseCursor(float DeltaTime)
{
	if (PlayerController)
	{
		float MouseX, MouseY;
		FVector WorldLocation, WorldDirection;

		PlayerController->GetMousePosition(MouseX, MouseY);
		PlayerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

		/** setup reticle location and opacity */
		float RoughtStickMagnitude = FMath::Max(FMath::Abs(WorldDirection.X), FMath::Abs(WorldDirection.Y));
		if (ReticleDynamicMaterial)
		{
			ReticleDynamicMaterial->SetScalarParameterValue("Opacity", RoughtStickMagnitude);
		}

		FVector VectorToRotate = FVector(WorldDirection.X, -WorldDirection.Y, 0.f);
		FRotator Rotation = FRotationMatrix::MakeFromX(VectorToRotate).Rotator();

		FVector ReticleDistanceVector = FVector(ReticleDistance, 0.f, 0.f);

		ReticleLocation->SetRelativeLocation(Rotation.UnrotateVector(ReticleDistanceVector));

		/** rotate wings towards cursor  */
		FVector From = ReticleLocation->RelativeLocation;
		FVector To = WeaponWings->RelativeLocation;
		FRotator WingsDesiredRotatation = FRotationMatrix::MakeFromX((To - From).GetSafeNormal()).Rotator();

		FRotator RInterpToRotator = FMath::RInterpTo(WeaponWings->RelativeRotation, WingsDesiredRotatation, DeltaTime, InterpSpeed);
		WeaponWings->SetRelativeRotation(UKismetMathLibrary::RLerp(WeaponWings->RelativeRotation, RInterpToRotator, RoughtStickMagnitude, true));
	}
}

FLinearColor AGeo::GetCurrentWeaponColor() const
{
	if (WeaponColors.IsValidIndex(CurrentWeapon))
	{
		return WeaponColors[CurrentWeapon];
	}
	else
	{
		return FLinearColor::MakeRandomColor();
	}
}
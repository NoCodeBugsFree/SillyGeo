// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Curves/CurveFloat.h"
#include "Geo.generated.h"

UCLASS()
class SILLYGEO_API AGeo : public ACharacter
{
	GENERATED_BODY()

	/** players wings scene  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Wings;

	/** ?  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* PreRotateWings;

	/** camera attach point  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CameraRoot;

	/** reticle location scene  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* ReticleLocation;

	/** wings mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WeaponWings;

	/** the player body mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Puck;

	/** players trail  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* Trail;

	/** players top down camera  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCamera;

	/** world global sparks FX  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* Sparks;

	/** reticle mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Reticle;
	
	/** sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* FireSound;

	/** all wings static meshes  */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class UStaticMesh*> WingList;

	/** the curve for zoom in/out camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* CameraCurve;

	
public:

	/** Apply damage to this actor.*/
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	/** returns the color of current weapon  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	FLinearColor GetCurrentWeaponColor() const;

protected:
	
	// Sets default values for this character's properties
	AGeo();

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** calls to change weapon meshes by timeline */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void WeaponSwitchFX();
	
private:

	/** calls to move X-axis  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MovementX(float Value);

	/** calls to move Y-axis  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void MovementY(float Value);

	/** [tick] calls to zoom in/out camera from player according to players speed */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void ZoomCamera();
	
	/** [tick] calls to rotate weapon to mouse cursor  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RotateToMouseCursor(float DeltaTime);

	/** calls to set wings and trail color according to current weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetWingsAndTrailColor();
	
	/** calls to switch weapon to next weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void NextWeapon();

	/** calls to switch weapon to previous weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void PreviousWeapon();

	/** calls to switch weapon to next or previous weapon */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SwitchWeapon(int32 WeaponStep);

	/** calls to init player when it begins play or respawn */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void InitializePlayer();

	/** calls per RegenTimer delay to constantly regenerate characters health  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void RegenerateHealth();

	/** calls to start firing as fast as we can  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartFire();

	/** calls to stop firing  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StopFire();

	/** internal call for instant fire */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Fire();

	// -----------------------------------------------------------------------------------
	
	/** interp rotation speed from current wings rotation to reticle */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float InterpSpeed = 10.f;

	/** player controller reference  */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	APlayerController* PlayerController;

	/** the color of weapon to save to further lerp from this color to the 
	*	color of the next weapon
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor PreviousWeaponColor = FLinearColor::Black;

	/** shows whether we can fire right now or not */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCanFire : 1;

	/** shows whether we can switch weapon right now or not */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bCanSwitchWeapon : 1;

	/** the delay (in sec) between shoots  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float FireRate = 0.25f;

	/** the max distance from original location to zoom camera when we start movement  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float CameraZoomMax = 500.f;

	/** the distance in uu (cm) from player to reticle  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float ReticleDistance = 200.f;
	
	UPROPERTY()
	FTimerHandle FireTimer;

	/** projectile template to spawn when fire  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AProjectile> ProjectileTemplate;

	/** shows whether we use left or right muzzle socket to fire
	*	every shot will flipflop this variable
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bLeftMuzzle : 1;
	
	/** dynamic material for player wings  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* WingsDynamicMaterial;

	/** dynamic material for reticle  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* ReticleDynamicMaterial;
	
	/** a list of available weapon colors  */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true", EditCondition = "!bUseDefaultColors"))
	TArray<FLinearColor> WeaponColors;

	/** the number of current weapon
	*   some other player stuff depends on this parameter
	*   e.g. wings color, trail color, projectile color and weapon type
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWeapon = 0;

	/** time between health regeneration   */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float RegenerationDelay = 0.125f;

	/** represents characters current health  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Health = 200.f;

	/** represents characters maximum health  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 200.f;

	/** player controller reference  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APlayerController* PC;

	/** game state reference  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AGeoGameState* GeoGameState;
	
	/** constantly triggers to regenerate characters health  */
	UPROPERTY()
	FTimerHandle RegenTimer;

public:


};

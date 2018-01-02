// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyColor : uint8
{
	EN_Green UMETA(DisplayName = "Green"),
	EN_Red UMETA(DisplayName = "Red"),
	EN_Blue UMETA(DisplayName = "Blue"),
	EN_Yellow UMETA(DisplayName = "Yellow")
};

UCLASS()
class SILLYGEO_API AEnemyBase : public AActor
{
	GENERATED_BODY()

	/* hit sphere  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HitSphere;
	
	/* enemy static mesh  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* EnemyMesh;
	
	/* enemy movement  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* EnemyMovement;
	
	/** enemy mesh dynamic material  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInstanceDynamic* EnemyDynamicMaterial;

	/** enemy mesh dynamic material  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UMaterialInterface* CoreMaterial;
	
	/* explosion emitter template */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplosionEmitter;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class USoundBase* ExplosionSound;
	
public:
	
	/** Apply damage to this actor. */
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	/** calls when this actor spawned to set Game mode and Game state references  */
	void InitReferences(class ASillyGeoGameMode* NewGM, class AGeoGameState* NewGS);

	/** calls to set the target pawn for this enemy  */
	void SetTarget(class APawn* TargetPawn);

protected:

	// Sets default values for this actor's properties
	AEnemyBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/** sets a target to follow  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void InitTarget();

	/** starts enemies timers  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void StartTimers();

	/**  [tick] calls to follow the target */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Follow();

	/** calls when enemy is dead */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SpawnExplodeFX();

	/** calls by designer in construction script */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetDefaultValues(bool bNewSpinning = false, bool bShifting = false, EEnemyColor Color = EEnemyColor::EN_Red, class UMaterialInterface* Mat = nullptr, class UStaticMesh* Mesh = nullptr, float Speed = 400.f);
	
	/** calls when this actor overlaps by another   */
	UFUNCTION()
	void OnEnemyOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	/** calls to define target destination */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void Tracking();

	// -----------------------------------------------------------------------------------

	/** the color of enemy. this parameter specify enemy body color
	*	explosion FX color and sets this enemy to be vulnerable only for
	*	same projectile color
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FLinearColor CurrentColor;

	/** health amount  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float Health = 50.f;

	/** how strong this enemy hurts Geo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float DamageToCause = 50.f;

	/** the color of the enemy  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	EEnemyColor EnemyColor = EEnemyColor::EN_Red;

	/** the destination of our movement  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FVector Destination;

	/** shows how fast enemy is spinning  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float SpinRate = 2.f;

	/** TODO  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FVector RandomVector;

	/** the direction of random movement ( bRandomShift == true )  */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	FVector RandomDirection;
	
	/** shows whether enemy is spinning or not  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bSpinning : 1;

	/** shows whether enemy is randomly shifting or not  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bRandomShift : 1;

	/** the speed of enemy  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float EnemyMovementSpeed = 400.f;
	
	/** the delay between firing Tracking() function  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float TrackingDelay = 0.25f;
	
	/** game state reference  */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AGeoGameState* GeoGameState;

	/** game mode reference  */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class ASillyGeoGameMode* GeoGameMode;

	/** players pawn that we actually hunting  */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class APawn* PlayerPawn;
	
public:
	
	/** returns enemy color **/
	FORCEINLINE FLinearColor GetEnemyColor() const { return CurrentColor; }
	
};

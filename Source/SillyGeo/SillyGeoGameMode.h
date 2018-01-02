// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SillyGeo.h"
#include "GameFramework/GameMode.h"
#include "SillyGeoGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SILLYGEO_API ASillyGeoGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	/** calls to update HUD for each valid PC in the Game  */
	void UpdateHUD();

	/** calls to handle end wave condition, starts new wave or finish 
	*	the match according waves amount
	*/
	void EndWave();

	/** calls by spawner to store its reference here */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	void SetSpawnerReference(AEnemySpawner* SpawnerToSet);

	/** calls to check whether we have some enemies to spawn or not */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	bool HasEnemiesToSpawn() const;

	/** calls to obtain random player pawn as target to move to  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	class APawn* GetRandomPlayerPawn() const;
	
protected:

	virtual void BeginPlay() override;

	/** Called after a successful login.  This is the first place 
	*	it is safe to call replicated functions on the PlayerController.
	*	Saves the player controller reference to PlayerControllerList
	*/
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Called when a Controller with a PlayerState leaves the game or is destroyed 
	*	remove the exiting controller from the PlayerControllerList
	*/
	virtual void Logout(AController* Exiting) override;

	/**
	Transition from WaitingToStart to InProgress. You can call this manually,
	will also get called if ReadyToStartMatch returns true
	*/
	virtual void StartMatch() override;

	/**
	Transition from InProgress to WaitingPostMatch. You can call this manually,
	will also get called if ReadyToEndMatch returns true
	*/
	virtual void EndMatch() override;

	/**
	*	Initialize the GameState actor with default settings called
	during PreInitializeComponents() of the GameMode after
	a GameState has been spawned as well as during Reset()
	*/
	virtual void InitGameState() override;

	/** calls to update game state for next wave  */
	void BeginWave();

	/** calls to clear used early variables and start spawning */
	void BeginSpawning();

	/** calls to spawn an enemy if needed  */
	void SpawnEnemy();

private:

	// Editor code to make updating values in the editor cleaner
	#if WITH_EDITOR
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	#endif
	
	/** the list of all player controllers */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<class AGeoPlayerController*> PlayerControllerList;

	/** shows the delay between the waves spawning */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float WaveDelay = 5.f;

	/** shows the delay between the enemies spawning   */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float SpawnDelay = 1.5;

	/** maximum amount of waves to spawn   */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 MaxWaves;

	/** determines how many waves will be spawned and what stuff will contain each wave */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config", meta = (AllowPrivateAccess = "true"))
	TArray<struct FWaveInfo> WaveInfo;

	/**  the order number of enemy type to spawn */
	int32 EnemyToSpawn;

	/** stores the spawned enemies amount  */
	int32 EnemiesSpawned;

	/** wave delay timer  */
	UPROPERTY()
	FTimerHandle WaveTimerHandle;

	/** enemy delay timer  */
	UPROPERTY()
	FTimerHandle SpawnTimerHandle;

	/** stores the amount of all enemies of each particular enemy type   */
	TArray<int32> SpawnedOfType;

	/** game state reference  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AGeoGameState* GeoGameState;
	
	/** spawner reference  */
	UPROPERTY(BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	class AEnemySpawner* Spawner;
};

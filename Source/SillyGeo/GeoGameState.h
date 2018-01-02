// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GeoGameState.generated.h"

/**
 * 
 */
UCLASS()
class SILLYGEO_API AGeoGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	/** calls to activate/deactivate current wave  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetWaveActive(bool NewActive) { bWaveActive = NewActive; }

	/** calls to add the specified amount of enemies to remaining enemies counter */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void AddEnemiesRemaining(int32 Amount) { EnemiesRemaining += Amount; }

	/** sets the delay between waves  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetWaveDelay(float Delay) { WaveDelay = Delay; }

	/** sets the max waves amount  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetMaxWaves(int32 Waves) { MaxWaves = Waves; }

	/** sets the current wave number  */
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetCurrentWave(int32 Wave) { CurrentWave = Wave; }
	
	// -------------- H U D ------------------------------------------------------

	/** shows whether the current wave is active or not  */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	bool IsWaveActive() const { return bWaveActive; }
	
	/** returns the remaining enemies amount  */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	int32 GetEnemiesRemaining() const { return EnemiesRemaining; }

	/** returns the delay between ways  */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	float GetWaveDelay() const { return WaveDelay; }
	
	/** returns max waves / current wave  */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void GetWaves(int32& Max, int32& Current) const { Max = MaxWaves; Current = CurrentWave; }
	
private:

	/** shows how many enemies we need to kill  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 EnemiesRemaining;

	/** shows whether spawning is in process or not (rest between waves) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	uint32 bWaveActive : 1;

	/** the delay (in sec) to rest between waves  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	float WaveDelay = 10.f;

	/** the maximum waves amount */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 MaxWaves;

	/** the current wave number  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWave;

public:
	/** returns max amount of waves   */
	FORCEINLINE int32 GetMaxWaves() const { return MaxWaves; }
	/** returns current wave number  */
	FORCEINLINE int32 GetCurrentWave() const { return CurrentWave; }
};

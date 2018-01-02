// Fill out your copyright notice in the Description page of Project Settings.

#include "GeoGameState.h"
#include "Net/UnrealNetwork.h"

void AGeoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGeoGameState, EnemiesRemaining);
	DOREPLIFETIME(AGeoGameState, bWaveActive);
	DOREPLIFETIME(AGeoGameState, WaveDelay);
	DOREPLIFETIME(AGeoGameState, MaxWaves);
	DOREPLIFETIME(AGeoGameState, CurrentWave);
}

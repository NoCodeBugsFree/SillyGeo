// Fill out your copyright notice in the Description page of Project Settings.

#include "GeoPlayerState.h"
#include "Net/UnrealNetwork.h"

/** Returns properties that are replicated for the lifetime of the actor channel */
void AGeoPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AGeoPlayerState, EnemiesKilled);
}


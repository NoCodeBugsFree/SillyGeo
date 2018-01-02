// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "BitArray.h"
#include "SillyGeo.generated.h"

/**
*	specify the enemy template class and max amount of enemies of this type will
	be spawned during the wave
*/
USTRUCT(BlueprintType)
struct FSpawnInfo
{
	GENERATED_USTRUCT_BODY()

	/** enemy template class  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TSubclassOf<class AEnemyBase> EnemyTemplate = AEnemyBase::StaticClass();

	/** shows how many enemies of this type will be spawned during this wave
	*	( not greater than MaxEnemiesThisWave ! )
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	int32 MaxEnemiesAmount = 10;
};

/**
* per wave spawn info - wave intended to contain a list of all available enemies
  and their amount
*/
USTRUCT(BlueprintType)
struct FWaveInfo
{
	GENERATED_USTRUCT_BODY()

	/** shows how many enemies will be spawned during this wave  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AAA")
	int32 MaxEnemiesThisWave = 0;

	/** shows how many enemies of specified type will be spawned during this wave */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AAA")
	TArray<FSpawnInfo> SpawnInfo;
};
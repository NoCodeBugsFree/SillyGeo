// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GeoPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SILLYGEO_API AGeoPlayerState : public APlayerState
{
	GENERATED_BODY()
			
public:

	/** shows how many enemies was killed by our player  */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
	int32 EnemiesKilled = 0;
	
	
};

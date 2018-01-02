// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GeoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SILLYGEO_API AGeoPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** calls to update the HUD  */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void UpdateHUD();

	/** calls to handle win condition */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void WinTheGame();

	/** calls  to handle lose condition  */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AAA")
	void LoseTheGame();

protected:



private:
	
	
};

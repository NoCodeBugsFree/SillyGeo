// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"


UCLASS()
class SILLYGEO_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
	/* Box root component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxComponent;
	
public:	

	/** calls to spawn an enemy of specified class  */
	UFUNCTION(BlueprintCallable, Category = "AAA")
	class AEnemyBase* SpawnEnemy(TSubclassOf<class AEnemyBase> EnemyType);
	
protected:

	// Sets default values for this actor's properties
	AEnemySpawner();

	/** sets a self reference in game mode  */
	virtual void BeginPlay() override;

};

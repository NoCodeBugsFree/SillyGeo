// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "EnemyBase.h"
#include "SillyGeoGameMode.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	/* Box root  */
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	SetRootComponent(BoxComponent);
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if(ASillyGeoGameMode* SillyGeoGameMode = Cast<ASillyGeoGameMode>(GetWorld()->GetAuthGameMode()))
	{
		SillyGeoGameMode->SetSpawnerReference(this);
	}
}

AEnemyBase* AEnemySpawner::SpawnEnemy(TSubclassOf<class AEnemyBase> EnemyType)
{
	if (EnemyType)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			
			FVector SpawnOrigin = BoxComponent->Bounds.Origin;
			FVector SpawnExtent = BoxComponent->Bounds.BoxExtent;
			FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
			SpawnLocation.Z = 0.f;
			FRotator SpawnRotation = UKismetMathLibrary::RandomRotator();

			AEnemyBase* SpawnedEnemy = World->SpawnActor<AEnemyBase>(EnemyType, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedEnemy)
			{
				return SpawnedEnemy;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyType == NULL"));
	}
	return nullptr;
}



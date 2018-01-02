// Fill out your copyright notice in the Description page of Project Settings.
#include "SillyGeoGameMode.h"
#include "EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Geo.h"
#include "EnemySpawner.h"
#include "GeoGameState.h"
#include "GeoPlayerController.h"

void ASillyGeoGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateHUD();
}

void ASillyGeoGameMode::UpdateHUD()
{	
	for (AGeoPlayerController* GeoPC : PlayerControllerList)
	{
		if (GeoPC)
		{
			GeoPC->UpdateHUD();
		}
	}
}

void ASillyGeoGameMode::BeginWave()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	
	if (GeoGameState)
	{
		GeoGameState->SetWaveActive(true);
		GeoGameState->SetCurrentWave(GeoGameState->GetCurrentWave() + 1);
		UpdateHUD();
		BeginSpawning();
	}
}

void ASillyGeoGameMode::EndWave()
{
	if (GeoGameState)
	{
		GeoGameState->SetWaveActive(false);
		UpdateHUD();
		if (GeoGameState->GetCurrentWave() >= MaxWaves)
		{
			EndMatch();
		}
		else
		{
			GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASillyGeoGameMode::BeginWave, WaveDelay, false);
		}
	}
}

void ASillyGeoGameMode::BeginSpawning()
{
	if (GeoGameState)
	{
		SpawnedOfType.Empty();

		int32 CurrentWave = GeoGameState->GetCurrentWave();
		if (WaveInfo.IsValidIndex(CurrentWave - 1))
		{
			for (int32 i = 0; i < WaveInfo[CurrentWave - 1].SpawnInfo.Num(); i++)
			{
				SpawnedOfType.Add(0);
			}
			EnemyToSpawn = 0;
			EnemiesSpawned = 0;

			GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ASillyGeoGameMode::SpawnEnemy, SpawnDelay, true);
		}
	}
}

void ASillyGeoGameMode::SpawnEnemy()
{
	/** no spawner instance in the level  */
	if (!ensure(Spawner)) { return; }

	/** no waves  */
	if (WaveInfo.Num() < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("We need at least one wave! Set up the wave in Game Mode! "));
		GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
	}

	if (GeoGameState)
	{
		int32 CurrentWave = GeoGameState->GetCurrentWave();
		
		if (WaveInfo.IsValidIndex(CurrentWave - 1))
		{
			/** if enemies that we spawned during this wave is less than required  */
			if (EnemiesSpawned < WaveInfo[CurrentWave - 1].MaxEnemiesThisWave)
			{	
				if (WaveInfo[CurrentWave - 1].SpawnInfo.IsValidIndex(EnemyToSpawn))
				{
					/** spawn again  */
					FSpawnInfo SpawnInfo = WaveInfo[CurrentWave - 1].SpawnInfo[EnemyToSpawn];

					if (SpawnedOfType[EnemyToSpawn] < SpawnInfo.MaxEnemiesAmount)
					{
						if (SpawnInfo.EnemyTemplate)
						{
							AEnemyBase* SpawnedEnemy = Spawner->SpawnEnemy(SpawnInfo.EnemyTemplate);
							if (SpawnedEnemy)
							{
								SpawnedEnemy->InitReferences(this, GeoGameState);
								EnemiesSpawned++;
								SpawnedOfType[EnemyToSpawn]++;
								GeoGameState->AddEnemiesRemaining(1);
							}
						}
					}
				}

				/** if all types of enemies will be spawn - reset to the first enemy type  */
				if (EnemyToSpawn >= WaveInfo[CurrentWave - 1].SpawnInfo.Num() - 1)
				{
					EnemyToSpawn = 0;
				}
				else /** or go to the next type  */
				{
					EnemyToSpawn++;
				}
			}
			else /** we has spawned all enemies for this wave  */
			{
				GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
			}
		}
	}
	UpdateHUD();
}

void ASillyGeoGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/** add the controller to controllers list  */
	if(AGeoPlayerController* GeoPC = Cast<AGeoPlayerController>(NewPlayer))
	{
		PlayerControllerList.AddUnique(GeoPC);
	}
}

void ASillyGeoGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	/** remove the controller from  controllers list  */
	if (AGeoPlayerController* GeoPC = Cast<AGeoPlayerController>(Exiting))
	{
		PlayerControllerList.Remove(GeoPC);
	}
}

void ASillyGeoGameMode::StartMatch()
{
	// Returns true if the match state is InProgress or later
	if (!HasMatchStarted())
	{
		EndWave();
	}

	Super::StartMatch();
	UpdateHUD();
}

void ASillyGeoGameMode::EndMatch()
{
	Super::EndMatch();
	
	UpdateHUD();
	
	for (AGeoPlayerController* GeoPC : PlayerControllerList)
	{
		if (GeoPC)
		{
			GeoPC->WinTheGame();
		}
	}
}

void ASillyGeoGameMode::InitGameState()
{
	Super::InitGameState();

	GeoGameState = Cast<AGeoGameState>(UGameplayStatics::GetGameState(this));
	if (GeoGameState)
	{
		MaxWaves = WaveInfo.Num();
		GeoGameState->SetMaxWaves(MaxWaves);
		GeoGameState->SetWaveDelay(WaveDelay);
	}
}

void ASillyGeoGameMode::SetSpawnerReference(AEnemySpawner* SpawnerToSet)
{
	if (!ensure(SpawnerToSet)) { return; }
	Spawner = SpawnerToSet;
}

bool ASillyGeoGameMode::HasEnemiesToSpawn() const
{
	if (GeoGameState)
	{
		if (WaveInfo.IsValidIndex(GeoGameState->GetCurrentWave() - 1))
		{
			return EnemiesSpawned < WaveInfo[GeoGameState->GetCurrentWave() - 1].MaxEnemiesThisWave;
		}
	}
	return true;
}

class APawn* ASillyGeoGameMode::GetRandomPlayerPawn() const
{
	int32 RandInt = FMath::RandRange(0, PlayerControllerList.Num() - 1);
	if (PlayerControllerList.IsValidIndex(RandInt))
	{
		if (PlayerControllerList[RandInt])
		{
			return PlayerControllerList[RandInt]->GetPawn();
		}
	}
	return nullptr;
}

#if WITH_EDITOR
void ASillyGeoGameMode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	/** max waves update  */
	MaxWaves = WaveInfo.Num();
	
	/** for each wave update MaxEnemiesThisWave amount */
	for (int32 i = 0; i < WaveInfo.Num(); i++)
	{
		if (WaveInfo.IsValidIndex(i))
		{
			int32 MaxEnemies = 0;
			for (FSpawnInfo SpawnInfoItem : WaveInfo[i].SpawnInfo)
			{
				MaxEnemies += SpawnInfoItem.MaxEnemiesAmount;
			}
			WaveInfo[i].MaxEnemiesThisWave = MaxEnemies;
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif
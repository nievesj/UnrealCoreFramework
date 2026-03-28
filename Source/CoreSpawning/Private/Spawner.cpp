#include "Spawner.h"

#include "SpawnerConfigDataAsset.h"
#include "SpawnerFactory.h"
#include "ObjectPoolSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogSpawner);

ASpawner::ASpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASpawner::BeginPlay()
{
	Super::BeginPlay();

	if (const UWorld* World = GetWorld())
	{
		PoolSubsystem = World->GetSubsystem<UObjectPoolSubsystem>();
	}

	if (SpawnerConfig)
	{
		USpawnerFactory::InitializeFromConfig(PoolSubsystem, SpawnerConfig);

		if (SpawnerConfig->bAutoStart)
		{
			StartSpawning();
		}
	}
}

void ASpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	Super::EndPlay(EndPlayReason);
}

void ASpawner::StartSpawning()
{
	if (!SpawnerConfig)
	{
		UE_LOG(LogSpawner, Warning, TEXT("[%s] No SpawnerConfig set."), *GetName());
		return;
	}

	StopSpawning();

	GetWorldTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ASpawner::HandleSpawnTimer,
		SpawnerConfig->SpawnInterval,
		true,
		0.0f);
}

void ASpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

void ASpawner::HandleSpawnTimer()
{
	if (!SpawnerConfig)
	{
		return;
	}

	if (SpawnedActors.Num() >= SpawnerConfig->MaxAliveCount)
	{
		return;
	}

	SpawnOne();
}

void ASpawner::SpawnOne()
{
	if (!SpawnerConfig || !SpawnerConfig->SpawnClass)
	{
		return;
	}

	const FTransform Transform = GetSpawnTransform();
	AActor* Actor = USpawnerFactory::SpawnFromConfig(this, PoolSubsystem, SpawnerConfig, Transform);

	if (Actor)
	{
		SpawnedActors.Add(Actor);
		Actor->OnDestroyed.AddDynamic(this, &ASpawner::OnSpawnedActorDestroyed);
	}
}

FTransform ASpawner::GetSpawnTransform_Implementation()
{
	return GetActorTransform();
}

void ASpawner::OnSpawnedActorDestroyed(AActor* DestroyedActor)
{
	SpawnedActors.Remove(DestroyedActor);
}

void ASpawner::ReleaseOne(AActor* Actor)
{
	if (!Actor || !SpawnedActors.Contains(Actor))
	{
		return;
	}

	Actor->OnDestroyed.RemoveAll(this);
	SpawnedActors.Remove(Actor);

	if (SpawnerConfig)
	{
		USpawnerFactory::ReleaseFromConfig(PoolSubsystem, SpawnerConfig, Actor);
	}
	else
	{
		Actor->Destroy();
	}
}

void ASpawner::ReleaseAll()
{
	TArray<TObjectPtr<AActor>> ToRelease = MoveTemp(SpawnedActors);
	for (const TObjectPtr<AActor>& Actor : ToRelease)
	{
		if (IsValid(Actor))
		{
			Actor->OnDestroyed.RemoveAll(this);
			if (SpawnerConfig)
			{
				USpawnerFactory::ReleaseFromConfig(PoolSubsystem, SpawnerConfig, Actor);
			}
			else
			{
				Actor->Destroy();
			}
		}
	}
}


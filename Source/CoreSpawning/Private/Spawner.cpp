// MIT License
//
// Copyright (c) 2026 José M. Nieves
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

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

#pragma once

#include "GameFramework/Actor.h"

#include "Spawner.generated.h"

class USpawnerConfigDataAsset;
class UObjectPoolSubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogSpawner, Log, All);

/**
 * Configurable spawner driven by a data asset.
 * Uses FTimerHandle for spawn cadence (no Tick).
 * Optionally acquires actors from UObjectPoolSubsystem.
 */
UCLASS(Blueprintable)
class CORESPAWNING_API ASpawner : public AActor
{
	GENERATED_BODY()

public:
	ASpawner();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StartSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void StopSpawning();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnOne();

	/** Release a spawned actor back to the pool (or destroy it). */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void ReleaseOne(AActor* Actor);

	/** Release all spawned actors back to the pool (or destroy them). */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void ReleaseAll();

	UFUNCTION(BlueprintPure, Category = "Spawner")
	int32 GetAliveCount() const
	{
		return SpawnedActors.Num();
	}

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Override to customize spawn transform. Default: actor's own transform. */
	UFUNCTION(BlueprintNativeEvent, Category = "Spawner")
	FTransform GetSpawnTransform();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TObjectPtr<USpawnerConfigDataAsset> SpawnerConfig;

private:
	void HandleSpawnTimer();
	void OnSpawnedActorDestroyed(AActor* DestroyedActor);

	FTimerHandle SpawnTimerHandle;

	UPROPERTY(Transient)
	TObjectPtr<UObjectPoolSubsystem> PoolSubsystem;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};

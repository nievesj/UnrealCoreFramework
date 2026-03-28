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
	int32 GetAliveCount() const { return SpawnedActors.Num(); }

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


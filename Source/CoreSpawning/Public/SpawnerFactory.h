#pragma once

#include "UObject/Object.h"
#include "SpawnerConfigDataAsset.h"

#include "SpawnerFactory.generated.h"

class UObjectPoolSubsystem;

/**
 * Resolves spawn parameters from a USpawnerConfigDataAsset.
 * Centralizes pool creation and config resolution for ASpawner and ASpawnerVolume.
 */
UCLASS(BlueprintType)
class CORESPAWNING_API USpawnerFactory : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Initialize pools if the config uses pooling.
	 * Call once at BeginPlay from the owning spawner.
	 */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	static void InitializeFromConfig(UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config);

	/** Spawn or acquire an actor based on the config. */
	UFUNCTION(BlueprintCallable, Category = "Spawner", meta = (WorldContext = "WorldContext"))
	static AActor* SpawnFromConfig(UObject* WorldContext, UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config, const FTransform& SpawnTransform);

	/** Release an actor back to the pool (if pooled) or destroy it. */
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	static void ReleaseFromConfig(UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config, AActor* Actor);
};


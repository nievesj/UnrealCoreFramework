#pragma once

#include "Engine/DataAsset.h"

#include "SpawnerConfigDataAsset.generated.h"

/**
 * Data asset defining spawner configuration.
 * Used by ASpawner and ASpawnerVolume to drive spawn behavior.
 */
UCLASS(BlueprintType)
class CORESPAWNING_API USpawnerConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Actor class to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TSubclassOf<AActor> SpawnClass;

	/** Interval between spawns in seconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "0.1"))
	float SpawnInterval = 2.0f;

	/** Maximum number of alive actors from this spawner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "1"))
	int32 MaxAliveCount = 10;

	/** If true, acquire actors from an object pool instead of spawning fresh. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Pool")
	bool bUsePool = false;

	/** Name of the pool to use (must be registered with UObjectPoolSubsystem). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Pool", meta = (EditCondition = "bUsePool"))
	FName PoolName;

	/** Number of actors to pre-warm in the pool. Only used if bUsePool is true. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Pool", meta = (EditCondition = "bUsePool", ClampMin = "0"))
	int32 PoolPreWarmCount = 5;

	/** Max pool size. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner|Pool", meta = (EditCondition = "bUsePool", ClampMin = "1"))
	int32 MaxPoolSize = 32;

	/** Whether the spawner starts active on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bAutoStart = true;
};


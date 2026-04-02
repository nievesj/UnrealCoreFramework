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

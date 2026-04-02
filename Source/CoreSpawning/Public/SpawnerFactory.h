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

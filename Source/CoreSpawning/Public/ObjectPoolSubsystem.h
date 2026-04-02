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

#include "ObjectPool.h"
#include "Subsystems/WorldSubsystem.h"

#include "ObjectPoolSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectPoolSubsystem, Log, All);

/**
 * World subsystem managing named actor pools.
 * Pools are created with a unique FName key and can be accessed
 * from C++ (type-safe) or Blueprint (FName-keyed untyped API).
 */
UCLASS()
class CORESPAWNING_API UObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

	/** Create a named pool for the given actor class, pre-warming the specified count. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (DisplayName = "Create Actor Pool"))
	void CreateActorPool(FName PoolName, TSubclassOf<AActor> ActorClass, int32 PreWarmCount = 0, int32 MaxPoolSize = 64);

	/** Acquire an actor from the named pool. Returns nullptr if pool doesn't exist. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (DisplayName = "Acquire Actor From Pool"))
	AActor* AcquireActorFromPool(FName PoolName);

	/** Release an actor back to the named pool. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool", meta = (DisplayName = "Release Actor To Pool"))
	void ReleaseActorToPool(FName PoolName, AActor* Actor);

	/** Release all active actors in the named pool. */
	UFUNCTION(BlueprintCallable, Category = "ObjectPool")
	void ReleaseAllInPool(FName PoolName);

	/** Check if a named pool exists. */
	UFUNCTION(BlueprintPure, Category = "ObjectPool")
	bool DoesPoolExist(FName PoolName) const;

	/** Type-safe pool creation. */
	template <typename T>
	void CreatePool(FName PoolName, int32 PreWarmCount = 0, int32 MaxPoolSize = 64)
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value, "T must derive from AActor.");
		if (Pools.Contains(PoolName))
		{
			UE_LOG(LogObjectPoolSubsystem, Warning, TEXT("Pool [%s] already exists."), *PoolName.ToString());
			return;
		}

		TUniquePtr<FObjectPoolBase> Pool = MakeUnique<TObjectPool<T>>(MaxPoolSize);
		if (PreWarmCount > 0)
		{
			Pool->PreWarm(PreWarmCount, GetWorld());
		}
		Pools.Add(PoolName, MoveTemp(Pool));
	}

	/** Type-safe acquire. */
	template <typename T>
	T* AcquireFromPool(FName PoolName)
	{
		return static_cast<T*>(AcquireActorFromPool(PoolName));
	}

private:
	TMap<FName, TUniquePtr<FObjectPoolBase>> Pools;
};

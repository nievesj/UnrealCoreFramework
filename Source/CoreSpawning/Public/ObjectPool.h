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

#include "IPoolable.h"
#include "Templates/SubclassOf.h"
#include "Containers/Array.h"
#include "Containers/Set.h"

class UWorld;
class AActor;

DECLARE_LOG_CATEGORY_EXTERN(LogObjectPool, Log, All);

/**
 * Non-template base for actor object pools.
 * Stores inactive actors in an array and active actors in a set.
 * The subsystem stores pools as TUniquePtr<FObjectPoolBase>.
 */
class CORESPAWNING_API FObjectPoolBase
{
public:
	FObjectPoolBase() = default;
	explicit FObjectPoolBase(TSubclassOf<AActor> InActorClass, int32 InMaxPoolSize = 64);
	virtual ~FObjectPoolBase();

	/** Spawn InCount actors into the inactive pool. */
	void PreWarm(int32 InCount, UWorld* World);

	/** Acquire an actor from the pool. Spawns a new one if the pool is empty. */
	AActor* AcquireUntyped(UWorld* World);

	/** Release an actor back into the pool. */
	void ReleaseUntyped(AActor* Actor);

	/** Release all active actors back into the pool. */
	void ReleaseAll();

	/** Destroy all pooled and active actors. */
	void DestroyAll(UWorld* World);

	int32 GetActiveCount() const { return ActiveActors.Num(); }
	int32 GetInactiveCount() const { return InactiveActors.Num(); }
	TSubclassOf<AActor> GetActorClass() const { return ActorClass; }

protected:
	AActor* SpawnPooledActor(UWorld* World);
	void DeactivateActor(AActor* Actor);
	void ActivateActor(AActor* Actor);

	TSubclassOf<AActor> ActorClass;
	TArray<TWeakObjectPtr<AActor>> InactiveActors;
	TSet<TWeakObjectPtr<AActor>> ActiveActors;
	int32 MaxPoolSize = 64;
};

/**
 * Type-safe object pool for a specific actor class.
 */
template <typename T>
class TObjectPool : public FObjectPoolBase
{
	static_assert(TIsDerivedFrom<T, AActor>::Value, "TObjectPool only supports AActor subclasses.");

public:
	explicit TObjectPool(int32 InMaxPoolSize = 64)
		: FObjectPoolBase(T::StaticClass(), InMaxPoolSize)
	{
	}

	T* Acquire(UWorld* World)
	{
		return static_cast<T*>(AcquireUntyped(World));
	}

	void Release(T* Actor)
	{
		ReleaseUntyped(Actor);
	}
};


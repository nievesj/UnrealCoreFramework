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


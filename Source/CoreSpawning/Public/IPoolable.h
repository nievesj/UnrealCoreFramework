#pragma once

#include "UObject/Interface.h"

#include "IPoolable.generated.h"

/**
 * Interface for actors managed by an object pool.
 * Implement OnAcquired/OnReleased/ResetToPool to define
 * how the actor transitions between active and pooled states.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UPoolable : public UInterface
{
	GENERATED_BODY()
};

class CORESPAWNING_API IPoolable
{
	GENERATED_BODY()

public:
	/** Called when this actor is acquired from the pool. Restore to active state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	void OnAcquired();

	/** Called when this actor is released back to the pool. Deactivate and hide. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	void OnReleased();

	/** Reset all gameplay state so the actor can be reused cleanly. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pool")
	void ResetToPool();
};


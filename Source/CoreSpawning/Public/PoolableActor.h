#pragma once

#include "GameFramework/Actor.h"
#include "IPoolable.h"

#include "PoolableActor.generated.h"

/**
 * Base actor with default pool lifecycle behavior.
 * Disables collision, hides, and stops movement on release.
 * Re-enables everything on acquire.
 */
UCLASS(Blueprintable)
class CORESPAWNING_API APoolableActor : public AActor, public IPoolable
{
	GENERATED_BODY()

public:
	APoolableActor();

	virtual void OnAcquired_Implementation() override;
	virtual void OnReleased_Implementation() override;
	virtual void ResetToPool_Implementation() override;
};


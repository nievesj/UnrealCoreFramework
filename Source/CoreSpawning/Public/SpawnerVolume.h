#pragma once

#include "Spawner.h"

#include "SpawnerVolume.generated.h"

class UBoxComponent;

/**
 * Spawner that picks random points within a box volume.
 * Inherits spawn rate, pool integration, and data-asset config from ASpawner.
 */
UCLASS(Blueprintable)
class CORESPAWNING_API ASpawnerVolume : public ASpawner
{
	GENERATED_BODY()

public:
	ASpawnerVolume();

protected:
	virtual FTransform GetSpawnTransform_Implementation() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnerVolume")
	TObjectPtr<UBoxComponent> SpawnVolume;
};


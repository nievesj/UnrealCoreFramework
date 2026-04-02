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

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

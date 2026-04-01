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

#include "CoreGameFlowState.generated.h"

/**
 * Represents a named state in the game flow (MainMenu, Loading, InGame, Paused, GameOver).
 * Subclass and override OnEnter/OnExit for state-specific logic.
 */
UCLASS(Blueprintable, Abstract)
class UNREALCOREFRAMEWORK_API UCoreGameFlowState : public UObject
{
	GENERATED_BODY()

public:
	/** Called when entering this state. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnEnter();
	virtual void OnEnter_Implementation() {}

	/** Called when exiting this state. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnExit();
	virtual void OnExit_Implementation() {}

	/** Called each frame while this state is active. Override for per-frame logic. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnTick(float DeltaTime);
	virtual void OnTick_Implementation(float DeltaTime) {}

	/** Display name for this state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	FName StateName;
};


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

#include "CoreTweenBuilder.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SharedPointer.h"

#include "CoreTweenWorldSubsystem.generated.h"

class UWidget;
struct FCoreTweenState;

/**
 * Per-world storage for active tween states.
 * Replaces the former static TArray on UCoreTween so that PIE worlds
 * each get their own isolated set of tweens.
 */
UCLASS()
class CORETWEEN_API UCoreTweenWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Register a new tween state for tracking. Called by FCoreTweenBuilder::Run(). */
	void RegisterTweenState(TSharedPtr<FCoreTweenState> State);

	/** Cancel all active tweens on the given widget. Returns number cancelled. */
	int32 Clear(UWidget* Widget);

	/** @return true if any tween is actively running on this widget. */
	bool GetIsTweening(UWidget* Widget);

	/** Force-complete all active tweens. Each jumps to end state, fires OnComplete, and exits. */
	void CompleteAll();

	/** Accessor for the active tween states array. */
	TArray<TSharedPtr<FCoreTweenState>>& GetActiveTweenStates()
	{
		return ActiveTweenStates;
	}

private:
	/** Prune finished entries from the tracking array. */
	void PruneFinished();

	TArray<TSharedPtr<FCoreTweenState>> ActiveTweenStates;
};

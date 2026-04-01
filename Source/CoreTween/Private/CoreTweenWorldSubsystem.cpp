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

#include "CoreTweenWorldSubsystem.h"

#include "Components/Widget.h"

void UCoreTweenWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCoreTweenWorldSubsystem::Deinitialize()
{
	// Force-cancel all outstanding tweens so coroutines don't outlive the world.
	for (const TSharedPtr<FCoreTweenState>& State : ActiveTweenStates)
	{
		if (!State->bFinished.load(std::memory_order_acquire))
		{
			if (State->FlowState.IsValid())
			{
				State->FlowState->Cancel();
			}
			State->FlowState.Reset();
			State->bFinished.store(true, std::memory_order_release);
		}
	}
	ActiveTweenStates.Empty();

	Super::Deinitialize();
}

void UCoreTweenWorldSubsystem::RegisterTweenState(TSharedPtr<FCoreTweenState> State)
{
	PruneFinished();
	ActiveTweenStates.Add(MoveTemp(State));
}

int32 UCoreTweenWorldSubsystem::Clear(UWidget* Widget)
{
	int32 NumCancelled = 0;
	for (int32 Idx = ActiveTweenStates.Num() - 1; Idx >= 0; --Idx)
	{
		TSharedPtr<FCoreTweenState>& State = ActiveTweenStates[Idx];
		if (State->Widget.Get() == Widget)
		{
			if (!State->bFinished.load(std::memory_order_acquire) && State->FlowState.IsValid())
			{
				State->FlowState->Cancel();
			}
			State->FlowState.Reset();
			State->bFinished.store(true, std::memory_order_release);
			ActiveTweenStates.RemoveAtSwap(Idx);
			NumCancelled++;
		}
	}
	return NumCancelled;
}

bool UCoreTweenWorldSubsystem::GetIsTweening(UWidget* Widget)
{
	PruneFinished();
	for (const TSharedPtr<FCoreTweenState>& State : ActiveTweenStates)
	{
		if (State->Widget.Get() == Widget && !State->bFinished.load(std::memory_order_acquire))
		{
			return true;
		}
	}
	return false;
}

void UCoreTweenWorldSubsystem::CompleteAll()
{
	for (const TSharedPtr<FCoreTweenState>& State : ActiveTweenStates)
	{
		if (!State->bFinished.load(std::memory_order_acquire))
		{
			State->bForceComplete.store(true, std::memory_order_release);
		}
	}
}

void UCoreTweenWorldSubsystem::PruneFinished()
{
	ActiveTweenStates.RemoveAll([](const TSharedPtr<FCoreTweenState>& State)
	{
		return State->bFinished.load(std::memory_order_acquire);
	});
}

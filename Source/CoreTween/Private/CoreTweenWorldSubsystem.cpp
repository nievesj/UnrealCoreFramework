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

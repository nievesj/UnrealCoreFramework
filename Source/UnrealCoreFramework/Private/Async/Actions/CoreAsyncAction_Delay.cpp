#include "Async/Actions/CoreAsyncAction_Delay.h"

#include "AsyncFlowAwaiters.h"

UCoreAsyncAction_Delay* UCoreAsyncAction_Delay::AsyncDelay(UObject* InWorldContext, const float InDuration, const bool bInUnpaused)
{
	UCoreAsyncAction_Delay* Action = NewObject<UCoreAsyncAction_Delay>();
	Action->WorldContext = InWorldContext;
	Action->Duration = InDuration;
	Action->bUnpaused = bInUnpaused;
	Action->RegisterWithGameInstance(InWorldContext);
	return Action;
}

void UCoreAsyncAction_Delay::Activate()
{
	DelayTask = RunDelay();
}

AsyncFlow::TTask<void> UCoreAsyncAction_Delay::RunDelay()
{
	if (bUnpaused)
	{
		co_await AsyncFlow::UnpausedDelay(WorldContext, Duration);
	}
	else
	{
		co_await AsyncFlow::Delay(WorldContext, Duration);
	}

	OnComplete.Broadcast();
	SetReadyToDestroy();
}


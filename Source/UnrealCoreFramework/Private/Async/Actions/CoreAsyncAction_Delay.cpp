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


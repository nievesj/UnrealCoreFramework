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

#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncFlowTask.h"

#include "CoreAsyncAction_Delay.generated.h"

/**
 * Blueprint async action for AsyncFlow delays.
 * Supports game-time, real-time, and unpaused variants.
 */
UCLASS(meta = (ExposedAsyncProxy))
class UNREALCOREFRAMEWORK_API UCoreAsyncAction_Delay : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDelayComplete);

	UPROPERTY(BlueprintAssignable)
	FOnDelayComplete OnComplete;

	UFUNCTION(BlueprintCallable, Category = "AsyncFlow|Delay", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContext"))
	static UCoreAsyncAction_Delay* AsyncDelay(UObject* WorldContext, float Duration, bool bUnpaused = true);

	virtual void Activate() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContext;

	float Duration = 0.0f;
	bool bUnpaused = true;
	AsyncFlow::TTask<void> DelayTask;

	AsyncFlow::TTask<void> RunDelay();
};


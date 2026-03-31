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

#include "CoreAsyncAction_PlayWidgetTransition.generated.h"

class UCoreWidget;

/**
 * Blueprint async action for playing CoreWidget show/hide transitions.
 */
UCLASS(meta = (ExposedAsyncProxy))
class UNREALCOREFRAMEWORK_API UCoreAsyncAction_PlayWidgetTransition : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTransitionComplete);

	UPROPERTY(BlueprintAssignable)
	FOnTransitionComplete OnComplete;

	UFUNCTION(BlueprintCallable, Category = "AsyncFlow|Widget", meta = (BlueprintInternalUseOnly = "true"))
	static UCoreAsyncAction_PlayWidgetTransition* AsyncShowWidget(UCoreWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "AsyncFlow|Widget", meta = (BlueprintInternalUseOnly = "true"))
	static UCoreAsyncAction_PlayWidgetTransition* AsyncHideWidget(UCoreWidget* Widget);

	virtual void Activate() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UCoreWidget> TargetWidget;

	bool bIsShow = true;
	AsyncFlow::TTask<void> TransitionTask;

	AsyncFlow::TTask<void> RunTransition();
};


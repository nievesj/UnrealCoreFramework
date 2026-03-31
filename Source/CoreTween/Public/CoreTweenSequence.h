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
#include "AsyncFlowTask.h"
#include "Containers/Array.h"

class UObject;

/**
 * Sequential tween chain. Build a sequence of FCoreTweenBuilder steps,
 * then Run() to execute them one after another as a single awaitable task.
 *
 * Usage:
 *   co_await FCoreTweenSequence::Create()
 *       .Then(UCoreTween::Create(Widget, 0.3f).ToOpacity(1.0f))
 *       .Then(UCoreTween::Create(Widget, 0.2f).ToScale(FVector2D::UnitVector))
 *       .Run(this);
 */
class CORETWEEN_API FCoreTweenSequence
{
public:
	/** Create an empty sequence. */
	static FCoreTweenSequence Create();

	/** Append a tween step. Moves the builder into the sequence. */
	FCoreTweenSequence& Then(FCoreTweenBuilder InBuilder);

	/**
	 * Execute all steps sequentially. Each step runs to completion
	 * before the next begins.
	 * @param WorldContext  UObject used by AsyncFlow for world context.
	 * @return Awaitable task that resolves when the last step completes.
	 */
	AsyncFlow::TTask<void> Run(UObject* WorldContext);

private:
	TArray<FCoreTweenBuilder> Steps;
};


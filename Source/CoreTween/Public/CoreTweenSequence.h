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


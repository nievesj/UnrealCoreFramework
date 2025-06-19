// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Object.h"

#include "WaitUntilNextFrameTaskRunner.generated.h"

/**
 * Utility UObject that defers execution of a lambda until the next game frame.
 *
 * Usage:
 *   - Call Run() with a lambda to schedule it for execution on the next tick.
 *   - Returns a TFuture<void> that completes after the lambda runs.
 *   - The object manages its own lifetime and is garbage collected after execution.
 *
 * Implementation:
 *   - Inherits from UObject and FTickableGameObject to use Unreal's ticking system.
 *   - Uses TPromise/TFuture for async-style completion notification.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UWaitUntilNextFrameTaskRunner : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	using FLambda = TFunction<void()>;

	/**
	 * Schedules the provided lambda to execute on the next frame.
	 * @param InLambda The lambda to execute.
	 * @return A TFuture<void> that is fulfilled after the lambda runs.
	 */
	TFuture<void> Run(FLambda InLambda)
	{
		Lambda = MoveTemp(InLambda);
		Promise = MakeUnique<TPromise<void>>();
		AddToRoot(); // Prevent garbage collection until execution
		return Promise->GetFuture();
	}

	/**
	 * Allow cancellation of pending execution
	 */
	void Cancel()
	{
		if (Promise.IsValid())
		{
			Promise->SetValue(); // Or consider SetException for cancelled state
			Promise.Reset();
		}
		Lambda = nullptr;
		RemoveFromRoot();
	}

	/**
	 * Called every tick. Executes the lambda if set, fulfills the promise, and allows GC.
	 */
	virtual void Tick(float DeltaTime) override
	{
		if (Lambda)
		{
			Lambda();
			Lambda = nullptr;

			if (Promise.IsValid())
			{
				Promise->SetValue();
				Promise.Reset();
			}
			RemoveFromRoot(); // Allow object to be garbage collected
		}
	}

	/** Always tickable while alive. */
	virtual bool IsTickable() const override { return true; }

	/** Required by FTickableGameObject. */
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UNextFrameLambdaRunner, STATGROUP_Tickables); }

private:
	FLambda					   Lambda;	// Lambda to execute next frame
	TUniquePtr<TPromise<void>> Promise; // Promise to fulfill after execution
};

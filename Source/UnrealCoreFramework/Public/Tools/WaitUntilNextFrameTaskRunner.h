#pragma once

#include "UObject/Object.h"

#include "WaitUntilNextFrameTaskRunner.generated.h"

/**
 * Defers lambda execution until the next game frame.
 * Uses TPromise/TFuture for async completion notification.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UWaitUntilNextFrameTaskRunner : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	using FLambda = TFunction<void()>;

	/**
	 * Schedules lambda for next frame execution.
	 * @param InLambda Function to execute
	 * @return Future that completes after execution
	 */
	TFuture<void> Run(FLambda InLambda)
	{
		Lambda = MoveTemp(InLambda);
		Promise = MakeUnique<TPromise<void>>();
		AddToRoot(); // Prevent GC until execution
		return Promise->GetFuture();
	}

	/** Cancels pending execution and cleans up */
	void Cancel()
	{
		if (Promise.IsValid())
		{
			Promise->SetValue();
			Promise.Reset();
		}
		Lambda = nullptr;
		RemoveFromRoot();
	}

	/** Executes lambda and handles cleanup */
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
			RemoveFromRoot();
		}
	}

	/** Returns true while lambda is pending */
	virtual bool IsTickable() const override { return Lambda != nullptr; }

	/** Required for profiling */
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UWaitUntilNextFrameTaskRunner, STATGROUP_Tickables);
	}

private:
	/** Lambda to execute next frame */
	FLambda Lambda;

	/** Promise for completion notification */
	TUniquePtr<TPromise<void>> Promise;
};
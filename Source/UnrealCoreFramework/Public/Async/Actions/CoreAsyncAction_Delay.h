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


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


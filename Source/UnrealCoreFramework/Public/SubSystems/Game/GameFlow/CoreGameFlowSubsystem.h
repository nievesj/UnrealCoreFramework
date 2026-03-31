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

#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "AsyncFlowTask.h"

#include "CoreGameFlowSubsystem.generated.h"

class UCoreGameFlowState;
class UCoreGameFlowConfigDataAsset;

DECLARE_LOG_CATEGORY_EXTERN(LogGameFlow, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameFlowStateChanged, FName, StateName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameFlowTransition, FName, FromState, FName, ToState);

/**
 * Manages game flow state transitions (MainMenu → Loading → InGame → Paused → GameOver).
 * Persists across level loads as a GameInstanceSubsystem.
 * Transition graph is data-driven via UCoreGameFlowConfigDataAsset.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UCoreGameFlowSubsystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Request a state transition. Returns false if the transition is invalid. */
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	bool RequestTransition(FName TargetState);

	/** Async transition — co_await until the transition completes. */
	AsyncFlow::TTask<bool> TransitionTask(FName TargetState);

	/** Get the current state name. */
	UFUNCTION(BlueprintPure, Category = "GameFlow")
	FName GetCurrentStateName() const { return CurrentStateName; }

	/** Get the current state object. */
	UFUNCTION(BlueprintPure, Category = "GameFlow")
	UCoreGameFlowState* GetCurrentState() const { return CurrentState; }

	/** Check if a transition to the target state is valid. */
	UFUNCTION(BlueprintPure, Category = "GameFlow")
	bool IsTransitionValid(FName TargetState) const;

	/** Set the config data asset and rebuild the transition graph. */
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void SetConfig(UCoreGameFlowConfigDataAsset* InConfig);

	UPROPERTY(BlueprintAssignable, Category = "GameFlow")
	FOnGameFlowStateChanged OnStateEntered;

	UPROPERTY(BlueprintAssignable, Category = "GameFlow")
	FOnGameFlowStateChanged OnStateExited;

	UPROPERTY(BlueprintAssignable, Category = "GameFlow")
	FOnGameFlowTransition OnTransitionStarted;

	UPROPERTY(BlueprintAssignable, Category = "GameFlow")
	FOnGameFlowTransition OnTransitionCompleted;

private:
	void BuildTransitionGraph();
	void EnterState(FName StateName);
	void ExitCurrentState();
	UCoreGameFlowState* CreateStateInstance(FName StateName);

	UPROPERTY(Transient)
	TObjectPtr<UCoreGameFlowConfigDataAsset> Config;

	UPROPERTY(Transient)
	TObjectPtr<UCoreGameFlowState> CurrentState;

	FName CurrentStateName;
	bool bIsTransitioning = false;
	TArray<FName> QueuedTransitions;

	/** Map: StateName → StateClass */
	TMap<FName, TSubclassOf<UCoreGameFlowState>> StateClassMap;

	/** Set of valid (From, To) pairs */
	TSet<TPair<FName, FName>> ValidTransitions;

	/** Cached state instances */
	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UCoreGameFlowState>> StateInstances;
};


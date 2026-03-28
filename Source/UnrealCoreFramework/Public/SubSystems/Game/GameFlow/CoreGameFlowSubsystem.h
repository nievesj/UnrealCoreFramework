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


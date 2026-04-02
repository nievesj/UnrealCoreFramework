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

#include "SubSystems/Game/GameFlow/CoreGameFlowSubsystem.h"

#include "SubSystems/Game/GameFlow/CoreGameFlowState.h"
#include "SubSystems/Game/GameFlow/CoreGameFlowConfigDataAsset.h"
#include "Async/CoreAsyncTypes.h"
#include "AsyncFlowAwaiters.h"

DEFINE_LOG_CATEGORY(LogGameFlow);

void UCoreGameFlowSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UCoreGameFlowSubsystem::Deinitialize()
{
	if (CurrentState)
	{
		CurrentState->OnExit();
		CurrentState = nullptr;
	}
	StateInstances.Empty();
	Super::Deinitialize();
}

void UCoreGameFlowSubsystem::SetConfig(UCoreGameFlowConfigDataAsset* InConfig)
{
	Config = InConfig;
	BuildTransitionGraph();

	if (Config && !Config->InitialState.IsNone())
	{
		EnterState(Config->InitialState);
	}
}

void UCoreGameFlowSubsystem::BuildTransitionGraph()
{
	StateClassMap.Empty();
	ValidTransitions.Empty();

	if (!Config)
	{
		return;
	}

	for (const FCoreGameFlowStateDefinition& StateDef : Config->States)
	{
		if (!StateDef.StateName.IsNone() && StateDef.StateClass)
		{
			StateClassMap.Add(StateDef.StateName, StateDef.StateClass);
		}
	}

	for (const FCoreGameFlowTransitionDefinition& TransDef : Config->Transitions)
	{
		ValidTransitions.Add(TPair<FName, FName>(TransDef.FromState, TransDef.ToState));
	}
}

bool UCoreGameFlowSubsystem::IsTransitionValid(const FName TargetState) const
{
	if (CurrentStateName.IsNone())
	{
		return StateClassMap.Contains(TargetState);
	}
	return ValidTransitions.Contains(TPair<FName, FName>(CurrentStateName, TargetState));
}

bool UCoreGameFlowSubsystem::RequestTransition(const FName TargetState)
{
	if (bIsTransitioning)
	{
		QueuedTransitions.Add(TargetState);
		return true;
	}

	if (!IsTransitionValid(TargetState))
	{
		UE_LOG(LogGameFlow, Warning, TEXT("Invalid transition from [%s] to [%s]."), *CurrentStateName.ToString(), *TargetState.ToString());
		return false;
	}

	bIsTransitioning = true;
	const FName PreviousState = CurrentStateName;

	OnTransitionStarted.Broadcast(PreviousState, TargetState);

	ExitCurrentState();
	EnterState(TargetState);

	OnTransitionCompleted.Broadcast(PreviousState, TargetState);
	bIsTransitioning = false;

	// Process queued transitions
	if (QueuedTransitions.Num() > 0)
	{
		const FName NextState = QueuedTransitions[0];
		QueuedTransitions.RemoveAt(0);
		RequestTransition(NextState);
	}

	return true;
}

AsyncFlow::TTask<bool> UCoreGameFlowSubsystem::TransitionTask(const FName TargetState)
{
	UCF_ASYNC_CONTRACT(this);

	const bool bResult = RequestTransition(TargetState);
	co_return bResult;
}

void UCoreGameFlowSubsystem::EnterState(const FName StateName)
{
	CurrentStateName = StateName;
	CurrentState = CreateStateInstance(StateName);

	if (CurrentState)
	{
		CurrentState->OnEnter();
	}

	OnStateEntered.Broadcast(StateName);
}

void UCoreGameFlowSubsystem::ExitCurrentState()
{
	if (CurrentState)
	{
		CurrentState->OnExit();
		OnStateExited.Broadcast(CurrentStateName);
	}
}

UCoreGameFlowState* UCoreGameFlowSubsystem::CreateStateInstance(const FName StateName)
{
	// Return cached instance if already created
	if (TObjectPtr<UCoreGameFlowState>* Found = StateInstances.Find(StateName))
	{
		return *Found;
	}

	const TSubclassOf<UCoreGameFlowState>* StateClass = StateClassMap.Find(StateName);
	if (!StateClass || !*StateClass)
	{
		UE_LOG(LogGameFlow, Warning, TEXT("No state class registered for [%s]."), *StateName.ToString());
		return nullptr;
	}

	UCoreGameFlowState* NewState = NewObject<UCoreGameFlowState>(this, *StateClass);
	NewState->StateName = StateName;
	StateInstances.Add(StateName, NewState);

	return NewState;
}

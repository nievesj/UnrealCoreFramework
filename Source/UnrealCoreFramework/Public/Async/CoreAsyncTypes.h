// CoreAsyncTypes.h — AsyncFlow conventions for UnrealCoreFramework
//
// All coroutine-owning classes in UCF must follow these conventions:
//
// 1. TASK OWNERSHIP
//    Every UObject that launches a TTask stores it as a member variable.
//    This prevents the coroutine frame from being destroyed prematurely.
//
// 2. LIFETIME CONTRACTS
//    Every coroutine's first statement is a CO_CONTRACT capturing a
//    TWeakObjectPtr of the owning UObject. Raw `this` capture in deferred
//    lambdas is forbidden.
//
// 3. CANCELLATION ON TEARDOWN
//    Subsystems call Task.Cancel() in Deinitialize() for each owned task.
//    Components call it in EndPlay().
//
// 4. DEBUG NAMES
//    Every framework-launched task gets a descriptive debug name:
//      DataAssetLoad_<AssetId>
//      LevelLoad_<Name>
//      HttpRequest_<Verb>_<Url>
//      WidgetIntro_<WidgetName>
//
// 5. DEBUGGER REGISTRATION
//    Long-lived tasks (level loads, HTTP requests) call
//    AsyncFlow::DebugRegisterTask() on start and
//    AsyncFlow::DebugUnregisterTask() on completion or cancellation.
//
// 6. PUBLIC HEADER STRATEGY
//    Coroutine return types (AsyncFlow::TTask<T>) appear in public headers.
//    Include "AsyncFlowTask.h" (minimal header) in .h files.
//    Include "AsyncFlow.h" (umbrella) only in .cpp files.
#pragma once

#include "AsyncFlowTask.h"
#include "AsyncFlowMacros.h"

/**
 * Convenience macro for the standard UCF coroutine contract pattern.
 * Captures a TWeakObjectPtr of the owning UObject and registers a contract
 * that cancels the coroutine if the owner is destroyed.
 *
 * Usage (first line inside a coroutine body):
 *   UCF_ASYNC_CONTRACT(this);
 */
#define UCF_ASYNC_CONTRACT(OwnerPtr) \
	do \
	{ \
		TWeakObjectPtr<UObject> WeakOwner_Contract(OwnerPtr); \
		CO_CONTRACT([WeakOwner_Contract]() { return WeakOwner_Contract.IsValid(); }); \
	} while (false)


// Fill out your copyright notice in the Description page of Project Settings.

#include "SubSystems/Base/CoreWorldSubsystem.h"

DEFINE_LOG_CATEGORY(LogCoreWorldSubsystem);

void UCoreWorldSubsystem::PreInitialize()
{
}

void UCoreWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	PreInitialize();
	Super::Initialize(Collection);
	UE_LOG(LogCoreWorldSubsystem, Log, TEXT("Initialize UCoreWorldSubsystem"));
}

void UCoreWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCoreWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UCoreWorldSubsystem::OnWorldComponentsUpdated(UWorld& World)
{
	Super::OnWorldComponentsUpdated(World);
}

void UCoreWorldSubsystem::UpdateStreamingState()
{
	// Super::UpdateStreamingState();
}

void UCoreWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogCoreWorldSubsystem, Log, TEXT("Initialize UCoreWorldSubsystem"));
}

/*TFuture<void> UCoreWorldSubsystem::SlowTaskAsync()
{
	return Async(
			   EAsyncExecution::TaskGraphMainThread,
			   []()
			   {
				   // ExecuteSlowTask();
			   }
	)
		.Then(
			[](auto Future)
			{
				// DoSomethingElseAfterward();
			}
		);
}*/
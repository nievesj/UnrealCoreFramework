#include "SubSystems/LevelManagerSubsystem.h"

#include "Async/CoreAsyncTypes.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AsyncFlow.h"
#include "AsyncFlowLevelAwaiters.h"
#include "AsyncFlowDebug.h"

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelManagerSubsystem::Deinitialize()
{
	for (TPair<FString, AsyncFlow::TTask<bool>>& Pair : ActiveLevelTasks)
	{
		if (Pair.Value.IsValid() && !Pair.Value.IsCompleted())
		{
			Pair.Value.Cancel();
		}
	}
	ActiveLevelTasks.Empty();
	LoadingLevels.Empty();

	Super::Deinitialize();
}

void ULevelManagerSubsystem::LoadLevel(const FString& LevelPath,
	ELevelOperation Operation,
	ELevelLoadMethod LoadMethod)
{
	AsyncFlow::TTask<bool> Task = LoadLevelTask(LevelPath, Operation);
	const FString DebugName = FString::Printf(TEXT("LevelLoad_%s"), *LevelPath);
	Task.SetDebugName(DebugName);
	AsyncFlow::DebugRegisterTask(Task);
	Task.Start();
	ActiveLevelTasks.Add(LevelPath, MoveTemp(Task));
}

void ULevelManagerSubsystem::LoadLevelSoftObject(const TSoftObjectPtr<UWorld>& Level,
	ELevelOperation Operation,
	ELevelLoadMethod LoadMethod)
{
	if (!Level.IsValid() && Level.IsNull())
	{
		OnLevelLoadingError.Broadcast(TEXT("Unknown"), TEXT("Invalid level reference"));
		return;
	}

	LoadLevel(Level.GetAssetName(), Operation, LoadMethod);
}

AsyncFlow::TTask<bool> ULevelManagerSubsystem::LoadLevelTask(const FString& LevelPath, ELevelOperation Operation)
{
	UCF_ASYNC_CONTRACT(this);

	if (!ValidateLevelPath(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Invalid level path"));
		co_return false;
	}

	if (IsLevelLoading(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Level is already being loaded"));
		co_return false;
	}

	OnLevelLoadingStarted.Broadcast(LevelPath);
	LoadingLevels.Add(LevelPath, true);

	if (Operation == ELevelOperation::Open)
	{
		// Full map travel — world is destroyed, coroutine will not resume.
		// Fire the delegate before opening so listeners get notified.
		UGameplayStatics::OpenLevel(this, *LevelPath, true);

		// If we reach here (synchronous open or fallback), mark complete.
		LoadingLevels.Add(LevelPath, false);
		OnLevelLoadingComplete.Broadcast(LevelPath);
		co_return true;
	}
	else
	{
		// Stream — use AsyncFlow level streaming awaiter
		const bool bSuccess = co_await AsyncFlow::LoadStreamLevel(this, FName(*LevelPath), true);

		LoadingLevels.Add(LevelPath, false);

		if (bSuccess)
		{
			OnLevelLoadingComplete.Broadcast(LevelPath);
		}
		else
		{
			OnLevelLoadingError.Broadcast(LevelPath, TEXT("Stream level load failed"));
		}

		co_return bSuccess;
	}
}

AsyncFlow::TTask<bool> ULevelManagerSubsystem::LoadLevelTask(const TSoftObjectPtr<UWorld>& Level, ELevelOperation Operation)
{
	UCF_ASYNC_CONTRACT(this);

	if (!Level.IsValid() && Level.IsNull())
	{
		OnLevelLoadingError.Broadcast(TEXT("Unknown"), TEXT("Invalid level reference"));
		co_return false;
	}

	const bool bResult = co_await LoadLevelTask(Level.GetAssetName(), Operation);
	co_return bResult;
}

AsyncFlow::TTask<bool> ULevelManagerSubsystem::UnloadLevelTask(const FString& LevelPath)
{
	UCF_ASYNC_CONTRACT(this);

	if (!ValidateLevelPath(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Invalid level path"));
		co_return false;
	}

	if (!IsLevelLoaded(LevelPath))
	{
		co_return false;
	}

	const bool bSuccess = co_await AsyncFlow::UnloadStreamLevel(this, FName(*LevelPath));
	co_return bSuccess;
}

void ULevelManagerSubsystem::UnloadLevel(const FString& LevelPath)
{
	AsyncFlow::TTask<bool> Task = UnloadLevelTask(LevelPath);
	const FString DebugName = FString::Printf(TEXT("LevelUnload_%s"), *LevelPath);
	Task.SetDebugName(DebugName);
	Task.Start();
	ActiveLevelTasks.Add(LevelPath, MoveTemp(Task));
}

bool ULevelManagerSubsystem::IsLevelLoaded(const FString& LevelPath) const
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(World, FName(*LevelPath));
	return (StreamingLevel != nullptr) && StreamingLevel->IsLevelLoaded();
}

bool ULevelManagerSubsystem::IsLevelLoading(const FString& LevelPath) const
{
	return LoadingLevels.Contains(LevelPath) && LoadingLevels[LevelPath];
}

bool ULevelManagerSubsystem::ValidateLevelPath(const FString& LevelPath)
{
	return !LevelPath.IsEmpty();
}



#include "SubSystems/LevelManagerSubsystem.h"

#include "Containers/UnrealString.h"
#include "CoreGlobals.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogVerbosity.h"
#include "Subsystems/SubsystemCollection.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/UObjectGlobals.h"

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void ULevelManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
	LoadingLevels.Empty();
}

void ULevelManagerSubsystem::LoadLevel(const FString& LevelPath,
	ELevelOperation									  Operation,
	ELevelLoadMethod								  LoadMethod)
{
	LoadLevel(LevelPath, FLevelLoadingCallback(), Operation, LoadMethod);
}

void ULevelManagerSubsystem::LoadLevelSoftObject(const TSoftObjectPtr<UWorld>& Level,
	ELevelOperation															   Operation,
	ELevelLoadMethod														   LoadMethod)
{
	LoadLevel(Level, FLevelLoadingCallback(), Operation, LoadMethod);
}

void ULevelManagerSubsystem::LoadLevel(const FString& LevelPath,
	const FLevelLoadingCallback&					  Callback,
	ELevelOperation									  Operation,
	ELevelLoadMethod								  LoadMethod)
{
	if (!ValidateLevelPath(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Invalid level path"));
		if (Callback.IsBound())
		{
			Callback.Execute(false);
		}
		return;
	}

	if (IsLevelLoading(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Level is already being loaded"));
		if (Callback.IsBound())
		{
			Callback.Execute(false);
		}
		return;
	}

	if (Callback.IsBound())
	{
		LoadingCallbacks.Add(LevelPath, Callback);
	}

	OnLevelLoadingStarted.Broadcast(LevelPath);
	LoadingLevels.Add(LevelPath, true);

	if (Operation == ELevelOperation::Open)
	{
		// For opening levels, we need to use a different mechanism
		// Store the level path for use in the callback
		PendingLevelPath = LevelPath;

		// Register the delegate for level load completion
		LevelLoadHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULevelManagerSubsystem::OnLevelOpenComplete);

		// Open the level with the requested method
		UGameplayStatics::OpenLevel(this, *LevelPath, LoadMethod == ELevelLoadMethod::Async);
	}
	else // Stream
	{
		// For streaming levels, we use a helper object with a latent action
		auto* CallbackHelper = NewObject<ULevelLoadingCallbackHelper>();
		CallbackHelper->Callback = Callback;
		CallbackHelper->LevelPath = LevelPath;
		CallbackHelper->LevelManager = this;

		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = CallbackHelper;
		LatentInfo.ExecutionFunction = "OnLevelLoaded";
		LatentInfo.UUID = FGuid::NewGuid().A;
		LatentInfo.Linkage = 0;

		UGameplayStatics::LoadStreamLevel(
			this,
			FName(*LevelPath),
			true,
			LoadMethod == ELevelLoadMethod::Async, // If Async is false, this will block
			LatentInfo);
	}
}

void ULevelManagerSubsystem::LoadLevel(const TSoftObjectPtr<UWorld>& Level,
	const FLevelLoadingCallback&									 Callback,
	ELevelOperation													 Operation,
	ELevelLoadMethod												 LoadMethod)
{
	if (!Level.IsValid())
	{
		OnLevelLoadingError.Broadcast(TEXT("Unknown"), TEXT("Invalid level reference"));
		if (Callback.IsBound())
		{
			Callback.Execute(false);
		}
		return;
	}

	LoadLevel(Level.GetAssetName(), Callback, Operation, LoadMethod);
}

void ULevelManagerSubsystem::UnloadLevel(const FString& LevelPath)
{
	if (!ValidateLevelPath(LevelPath))
	{
		OnLevelLoadingError.Broadcast(LevelPath, TEXT("Invalid level path"));
		return;
	}

	if (IsLevelLoaded(LevelPath))
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.UUID = FGuid::NewGuid().A;
		LatentInfo.Linkage = 0;

		UGameplayStatics::UnloadStreamLevel(
			this,
			FName(*LevelPath),
			LatentInfo,
			true // Block on unloading
		);
	}
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

void ULevelManagerSubsystem::HandleLoadingComplete(const FString& LevelPath)
{
	LoadingLevels.Add(LevelPath, false);
	OnLevelLoadingComplete.Broadcast(LevelPath);

	// Execute and remove the callback if it exists
	if (FLevelLoadingCallback* Callback = LoadingCallbacks.Find(LevelPath))
	{
		if (Callback->IsBound())
		{
			Callback->Execute(true);
		}
		LoadingCallbacks.Remove(LevelPath);
	}
}

void ULevelManagerSubsystem::OnLevelOpenComplete(UWorld* World)
{
	// Clean up the delegate handle
	// Clean up the delegate handle
	if (LevelLoadHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(LevelLoadHandle);
		LevelLoadHandle.Reset();
	}

	if (!PendingLevelPath.IsEmpty())
	{
		HandleLoadingComplete(PendingLevelPath);
		PendingLevelPath.Empty();
	}
}

bool ULevelManagerSubsystem::ValidateLevelPath(const FString& LevelPath)
{
	return !LevelPath.IsEmpty();
}
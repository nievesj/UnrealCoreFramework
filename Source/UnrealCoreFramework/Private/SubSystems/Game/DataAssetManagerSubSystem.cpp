#include "SubSystems/Game/DataAssetManagerSubSystem.h"

#include "Async/CoreAsyncTypes.h"
#include "Data/DamageTypeDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "AsyncFlow.h"
#include "AsyncFlowAssetAwaiters.h"

DEFINE_LOG_CATEGORY(LogDataAssetManagerSubsystem);

void UDataAssetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetManager = UAssetManager::GetIfInitialized();

	UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("DataAssetManagerSubsystem: Initialized"));

	TArray<UDataAsset*> DamageTypes = LoadAllDataAssetsOfType(UDamageTypeDataAsset::StaticClass());

	UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("DataAssetManagerSubsystem: Loaded %d DamageType assets"), DamageTypes.Num());
}

void UDataAssetManagerSubsystem::Deinitialize()
{
	for (TPair<FPrimaryAssetId, AsyncFlow::TTask<UDataAsset*>>& Pair : ActiveLoadTasks)
	{
		if (Pair.Value.IsValid() && !Pair.Value.IsCompleted())
		{
			Pair.Value.Cancel();
		}
	}
	ActiveLoadTasks.Empty();
	LoadedAssets.Empty();

	Super::Deinitialize();
}

UDataAsset* UDataAssetManagerSubsystem::LoadDataAssetByPrimaryAssetId(const FPrimaryAssetId AssetId)
{
	if (!AssetId.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: Invalid AssetId provided"));
		return nullptr;
	}

	if (UDataAsset* CachedAsset = nullptr; IsAssetCached(AssetId, CachedAsset))
	{
		if (IsValid(CachedAsset))
		{
			return CachedAsset;
		}
	}

	if (!IsValid(AssetManager))
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: AssetManager is not initialized"));
		return nullptr;
	}

	FSoftObjectPath AssetPath = AssetManager->GetPrimaryAssetPath(AssetId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("Invalid asset path for ID: %s"), *AssetId.ToString());
		return nullptr;
	}

	UObject* LoadedObject = AssetManager->GetStreamableManager().LoadSynchronous(AssetPath);
	if (!IsValid(LoadedObject))
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("Failed to load asset with ID: %s"), *AssetId.ToString());
	}

	UDataAsset* DataAsset = Cast<UDataAsset>(LoadedObject);
	if (!IsValid(DataAsset))
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("Failed to cast loaded object to UDataAsset with ID: %s"), *AssetId.ToString());
		return nullptr;
	}

	UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("Successfully loaded asset: %s"), *AssetId.ToString());
	LoadedAssets.Add(AssetId, DataAsset);

	return DataAsset;
}

UDataAsset* UDataAssetManagerSubsystem::LoadDataAssetByClass(const TSubclassOf<UDataAsset> AssetClass, const FString& AssetName)
{
	if (!AssetClass || AssetName.IsEmpty())
	{
		return nullptr;
	}

	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	const FPrimaryAssetId	AssetId(AssetType, FName(*AssetName));

	return LoadDataAssetByPrimaryAssetId(AssetId);
}

AsyncFlow::TTask<UDataAsset*> UDataAssetManagerSubsystem::LoadDataAssetAsyncTask(FPrimaryAssetId AssetId)
{
	UCF_ASYNC_CONTRACT(this);

	if (!AssetId.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("LoadDataAssetAsyncTask: Invalid AssetId"));
		OnDataAssetLoaded.Broadcast(nullptr, false);
		co_return nullptr;
	}

	// Return cached asset immediately
	if (UDataAsset* CachedAsset = nullptr; IsAssetCached(AssetId, CachedAsset))
	{
		if (IsValid(CachedAsset))
		{
			OnDataAssetLoaded.Broadcast(CachedAsset, true);
			co_return CachedAsset;
		}
	}

	UObject* LoadedObject = co_await AsyncFlow::AsyncLoadPrimaryAsset(AssetId);

	UDataAsset* DataAsset = Cast<UDataAsset>(LoadedObject);
	if (IsValid(DataAsset))
	{
		LoadedAssets.Add(AssetId, DataAsset);
		OnDataAssetLoaded.Broadcast(DataAsset, true);
		UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("Async loaded asset: %s"), *AssetId.ToString());
	}
	else
	{
		OnDataAssetLoaded.Broadcast(nullptr, false);
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("Async load failed for asset: %s"), *AssetId.ToString());
	}

	co_return DataAsset;
}

AsyncFlow::TTask<UDataAsset*> UDataAssetManagerSubsystem::LoadDataAssetByClassAsyncTask(
	TSubclassOf<UDataAsset> AssetClass, const FString& AssetName)
{
	UCF_ASYNC_CONTRACT(this);

	if (!AssetClass || AssetName.IsEmpty())
	{
		co_return nullptr;
	}

	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	const FPrimaryAssetId AssetId(AssetType, FName(*AssetName));

	UDataAsset* Result = co_await LoadDataAssetAsyncTask(AssetId);
	co_return Result;
}

AsyncFlow::TTask<TArray<UDataAsset*>> UDataAssetManagerSubsystem::LoadAllDataAssetsOfTypeAsyncTask(
	TSubclassOf<UDataAsset> AssetClass)
{
	UCF_ASYNC_CONTRACT(this);

	TArray<UDataAsset*> Results;
	if (!AssetClass)
	{
		co_return Results;
	}

	TArray<FPrimaryAssetId> AssetIds = GetAllAssetIds(AssetClass);
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		UDataAsset* Loaded = co_await LoadDataAssetAsyncTask(AssetId);
		if (IsValid(Loaded))
		{
			Results.Add(Loaded);
		}
	}

	co_return Results;
}

TArray<UDataAsset*> UDataAssetManagerSubsystem::LoadAllDataAssetsOfType(const TSubclassOf<UDataAsset> AssetClass)
{
	TArray<UDataAsset*> AssetsToLoad;
	if (!AssetClass)
	{
		return AssetsToLoad;
	}

	if (!IsValid(AssetManager))
	{
		return AssetsToLoad;
	}

	TArray<FPrimaryAssetId> AssetIds = GetAllAssetIds(AssetClass);
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		if (UDataAsset* LoadedAsset = LoadDataAssetByPrimaryAssetId(AssetId))
		{
			AssetsToLoad.Add(LoadedAsset);
		}
	}

	return AssetsToLoad;
}

TArray<FPrimaryAssetId> UDataAssetManagerSubsystem::GetAllAssetIds(const TSubclassOf<UDataAsset> AssetClass) const
{
	TArray<FPrimaryAssetId> AssetIds;
	if (!IsValid(AssetManager))
	{
		return AssetIds;
	}

	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	AssetManager->GetPrimaryAssetIdList(AssetType, AssetIds);

	return AssetIds;
}

bool UDataAssetManagerSubsystem::IsAssetLoaded(const FPrimaryAssetId& AssetId) const
{
	return LoadedAssets.Contains(AssetId) && IsValid(LoadedAssets[AssetId]);
}

void UDataAssetManagerSubsystem::PreloadAssets(const TArray<FPrimaryAssetId>& AssetIds)
{
	if (!IsValid(AssetManager))
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: AssetManager is not initialized"));
		return;
	}

	AssetManager->LoadPrimaryAssets(AssetIds);

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		if (UObject* LoadedObject = AssetManager->GetPrimaryAssetObject(AssetId))
		{
			if (UDataAsset* DataAsset = Cast<UDataAsset>(LoadedObject))
			{
				LoadedAssets.Add(AssetId, DataAsset);
			}
		}
	}
}

void UDataAssetManagerSubsystem::UnloadAsset(const FPrimaryAssetId AssetId)
{
	LoadedAssets.Remove(AssetId);

	if (AsyncFlow::TTask<UDataAsset*>* Task = ActiveLoadTasks.Find(AssetId))
	{
		if (Task->IsValid() && !Task->IsCompleted())
		{
			Task->Cancel();
		}
		ActiveLoadTasks.Remove(AssetId);
	}

	UAssetManager::Get().UnloadPrimaryAsset(AssetId);
}

void UDataAssetManagerSubsystem::ClearCache()
{
	LoadedAssets.Empty();

	for (TPair<FPrimaryAssetId, AsyncFlow::TTask<UDataAsset*>>& Pair : ActiveLoadTasks)
	{
		if (Pair.Value.IsValid() && !Pair.Value.IsCompleted())
		{
			Pair.Value.Cancel();
		}
	}
	ActiveLoadTasks.Empty();
}

bool UDataAssetManagerSubsystem::IsAssetCached(const FPrimaryAssetId& AssetId, UDataAsset*& OutCachedDataAsset) const
{
	if (const TObjectPtr<UDataAsset>* CachedAsset = LoadedAssets.Find(AssetId))
	{
		if (IsValid(CachedAsset->Get()))
		{
			UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("DataAssetManagerSubsystem: Loaded asset from cache: %s"), *AssetId.ToString());
			OutCachedDataAsset = CachedAsset->Get();
			return true;
		}
	}

	return false;
}

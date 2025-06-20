
#include "SubSystems/Game/DataAssetManagerSubSystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

DEFINE_LOG_CATEGORY(LogDataAssetManagerSubsystem);

void UDataAssetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogTemp, Log, TEXT("DataAssetManagerSubsystem: Initialized"));

	// Optionally preload critical assets here
	// PreloadCriticalAssets();
}

void UDataAssetManagerSubsystem::Deinitialize()
{
	// Clean up async handles
	for (auto& Handle : AsyncHandles)
	{
		if (Handle.Value.IsValid())
		{
			Handle.Value->CancelHandle();
		}
	}

	AsyncHandles.Empty();
	LoadedAssets.Empty();

	Super::Deinitialize();
}

UPrimaryDataAsset* UDataAssetManagerSubsystem::LoadDataAssetById(const FPrimaryAssetId AssetId)
{
	if (!AssetId.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: Invalid AssetId provided"));
		return nullptr;
	}

	// Check cache first
	if (const TObjectPtr<UPrimaryDataAsset>* CachedAsset = LoadedAssets.Find(AssetId))
	{
		if (IsValid(CachedAsset->Get()))
		{
			return CachedAsset->Get();
		}
	}

	// Load synchronously
	const UAssetManager& AssetManager = UAssetManager::Get();
	if (UObject* LoadedObject = AssetManager.GetPrimaryAssetObject(AssetId))
	{
		if (UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(LoadedObject))
		{
			LoadedAssets.Add(AssetId, DataAsset);
			return DataAsset;
		}
	}

	// Try to load from disk
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(AssetId);
	if (AssetPath.IsValid())
	{
		if (UObject* LoadedObject = AssetPath.TryLoad())
		{
			if (UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(LoadedObject))
			{
				LoadedAssets.Add(AssetId, DataAsset);
				return DataAsset;
			}
		}
	}

	UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: Failed to load asset with ID: %s"), *AssetId.ToString());
	return nullptr;
}

UPrimaryDataAsset* UDataAssetManagerSubsystem::LoadDataAssetByClass(const TSubclassOf<UPrimaryDataAsset> AssetClass, const FString& AssetName)
{
	if (!AssetClass || AssetName.IsEmpty())
	{
		return nullptr;
	}

	// Create asset ID from class and name
	FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UPrimaryDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	FPrimaryAssetId	  AssetId(AssetType, FName(*AssetName));

	return LoadDataAssetById(AssetId);
}

void UDataAssetManagerSubsystem::LoadDataAssetAsync(FPrimaryAssetId AssetId, TFunction<void(UPrimaryDataAsset*, bool)> OnLoaded)
{
	if (!AssetId.IsValid())
	{
		if (OnLoaded)
		{
			OnLoaded(nullptr, false);
		}
		return;
	}

	// Check cache first
	if (TObjectPtr<UPrimaryDataAsset>* CachedAsset = LoadedAssets.Find(AssetId))
	{
		if (IsValid(CachedAsset->Get()))
		{
			if (OnLoaded)
			{
				OnLoaded(CachedAsset->Get(), true);
			}
			return;
		}
	}

	// Load asynchronously
	UAssetManager&				  AssetManager = UAssetManager::Get();
	const TArray<FPrimaryAssetId> AssetIds = { AssetId };

	TSharedPtr<FStreamableHandle> Handle = AssetManager.LoadPrimaryAssets(
		AssetIds,
		TArray<FName>(),
		FStreamableDelegate::CreateLambda([this, AssetId, OnLoaded]() {
			this->HandleAssetLoadedFunction(AssetId, OnLoaded);
		}));

	if (Handle.IsValid())
	{
		AsyncHandles.Add(AssetId, Handle);
	}
	else
	{
		if (OnLoaded)
		{
			OnLoaded(nullptr, false);
		}
	}
}

void UDataAssetManagerSubsystem::LoadDataAssetByClassAsync(const TSubclassOf<UPrimaryDataAsset>& AssetClass, const FString& AssetName, const TFunction<void(UPrimaryDataAsset*, bool)>& OnLoaded)
{
	if (!AssetClass || AssetName.IsEmpty())
	{
		if (OnLoaded)
		{
			OnLoaded(nullptr, false);
		}
		return;
	}

	// Create asset ID from class and name
	FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UPrimaryDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	FPrimaryAssetId	  AssetId(AssetType, FName(*AssetName));

	LoadDataAssetAsync(AssetId, OnLoaded);
}

TArray<UPrimaryDataAsset*> UDataAssetManagerSubsystem::LoadAllDataAssetsOfType(const TSubclassOf<UPrimaryDataAsset> AssetClass)
{
	TArray<UPrimaryDataAsset*> AssetsToLoad;
	if (!AssetClass)
	{
		return AssetsToLoad;
	}

	const UAssetManager&	AssetManager = UAssetManager::Get();
	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UPrimaryDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(AssetType, AssetIds);

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		if (UPrimaryDataAsset* LoadedAsset = LoadDataAssetById(AssetId))
		{
			AssetsToLoad.Add(LoadedAsset);
		}
	}

	return AssetsToLoad;
}

void UDataAssetManagerSubsystem::LoadAllDataAssetsOfTypeAsync(const TSubclassOf<UPrimaryDataAsset>& AssetClass, const TFunction<void(UPrimaryDataAsset*, bool)>& OnEachLoaded)
{
	if (!AssetClass)
	{
		return;
	}

	UAssetManager&	  AssetManager = UAssetManager::Get();
	FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UPrimaryDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;

	TArray<FPrimaryAssetId> AssetIds;
	AssetManager.GetPrimaryAssetIdList(AssetType, AssetIds);

	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		LoadDataAssetAsync(AssetId, OnEachLoaded);
	}
}

TArray<FPrimaryAssetId> UDataAssetManagerSubsystem::GetAllAssetIds() const
{
	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(FPrimaryAssetType(), AssetIds);
	return AssetIds;
}

TArray<FPrimaryAssetId> UDataAssetManagerSubsystem::GetAssetIdsByType(const FPrimaryAssetType AssetType) const
{
	TArray<FPrimaryAssetId> AssetIds;
	UAssetManager::Get().GetPrimaryAssetIdList(AssetType, AssetIds);
	return AssetIds;
}

bool UDataAssetManagerSubsystem::IsAssetLoaded(const FPrimaryAssetId& AssetId) const
{
	return LoadedAssets.Contains(AssetId) && IsValid(LoadedAssets[AssetId]);
}

void UDataAssetManagerSubsystem::PreloadAssets(const TArray<FPrimaryAssetId>& AssetIds)
{
	UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.LoadPrimaryAssets(AssetIds);

	// Cache loaded assets
	for (const FPrimaryAssetId& AssetId : AssetIds)
	{
		if (UObject* LoadedObject = AssetManager.GetPrimaryAssetObject(AssetId))
		{
			if (UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(LoadedObject))
			{
				LoadedAssets.Add(AssetId, DataAsset);
			}
		}
	}
}

void UDataAssetManagerSubsystem::UnloadAsset(const FPrimaryAssetId AssetId)
{
	LoadedAssets.Remove(AssetId);

	// Cancel any pending async load
	if (const TSharedPtr<FStreamableHandle>* Handle = AsyncHandles.Find(AssetId))
	{
		if (Handle->IsValid())
		{
			(*Handle)->CancelHandle();
		}
		AsyncHandles.Remove(AssetId);
	}

	UAssetManager::Get().UnloadPrimaryAsset(AssetId);
}

void UDataAssetManagerSubsystem::ClearCache()
{
	LoadedAssets.Empty();

	for (auto& Handle : AsyncHandles)
	{
		if (Handle.Value.IsValid())
		{
			Handle.Value->CancelHandle();
		}
	}
	AsyncHandles.Empty();
}

void UDataAssetManagerSubsystem::HandleAssetLoadedFunction(const FPrimaryAssetId& AssetId, const TFunction<void(UPrimaryDataAsset*, bool)>& OnLoaded)
{
	// Remove from async handles
	AsyncHandles.Remove(AssetId);

	// Get the loaded asset
	UAssetManager& AssetManager = UAssetManager::Get();
	UObject*	   Asset = AssetManager.GetPrimaryAssetObject(AssetId);

	if (UPrimaryDataAsset* DataAsset = Cast<UPrimaryDataAsset>(Asset))
	{
		LoadedAssets.Add(AssetId, DataAsset);
		if (OnLoaded)
		{
			OnLoaded(DataAsset, true);
		}
		OnDataAssetLoaded.Broadcast(DataAsset, true);
	}
	else
	{
		if (OnLoaded)
		{
			OnLoaded(nullptr, false);
		}
		OnDataAssetLoaded.Broadcast(nullptr, false);
	}
}
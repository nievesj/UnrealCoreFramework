
#include "SubSystems/Game/DataAssetManagerSubSystem.h"

#include "Data/DamageTypeDataAsset.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

DEFINE_LOG_CATEGORY(LogDataAssetManagerSubsystem);

void UDataAssetManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	AssetManager = UAssetManager::GetIfInitialized();

	UE_LOG(LogTemp, Log, TEXT("DataAssetManagerSubsystem: Initialized"));

	TArray<UDataAsset*> DamageTypes = LoadAllDataAssetsOfType(UDamageTypeDataAsset::StaticClass());

	UE_LOG(LogTemp, Log, TEXT("DataAssetManagerSubsystem: Initialized"));
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

UDataAsset* UDataAssetManagerSubsystem::LoadDataAssetByPrimaryAssetId(const FPrimaryAssetId AssetId)
{
	if (!AssetId.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("DataAssetManagerSubsystem: Invalid AssetId provided"));
		return nullptr;
	}

	// Check cache first
	if (UDataAsset* CachedAsset = nullptr; IsAssetCached(AssetId, CachedAsset))
	{
		if (IsValid(CachedAsset))
		{
			UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("DataAssetManagerSubsystem: Loaded asset from cache: %s "), *AssetId.ToString());
			return CachedAsset;
		}
	}

	// Get the asset path and load synchronously
	FSoftObjectPath AssetPath = AssetManager->GetPrimaryAssetPath(AssetId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(LogDataAssetManagerSubsystem, Warning, TEXT("Invalid asset path for ID: %s"), *AssetId.ToString());
		return nullptr;
	}

	// Load synchronously using StreamableManager
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

	// Create asset ID from class and name
	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	const FPrimaryAssetId	AssetId(AssetType, FName(*AssetName));

	return LoadDataAssetByPrimaryAssetId(AssetId);
}

void UDataAssetManagerSubsystem::LoadDataAssetAsync(FPrimaryAssetId AssetId, TFunction<void(UDataAsset*, bool)> OnLoaded)
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
	if (UDataAsset* CachedAsset = nullptr; IsAssetCached(AssetId, CachedAsset))
	{
		if (IsValid(CachedAsset))
		{
			if (OnLoaded)
			{
				OnLoaded(CachedAsset, true);
			}
			return;
		}
	}

	const TArray<FPrimaryAssetId>		AssetIds = { AssetId };
	const TSharedPtr<FStreamableHandle> Handle = AssetManager->LoadPrimaryAssets(AssetIds, TArray<FName>(),
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

void UDataAssetManagerSubsystem::LoadDataAssetByClassAsync(const TSubclassOf<UDataAsset>& AssetClass, const FString& AssetName, const TFunction<void(UDataAsset*, bool)>& OnLoaded)
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
	const FPrimaryAssetType AssetType = AssetClass->GetDefaultObject<UDataAsset>()->GetPrimaryAssetId().PrimaryAssetType;
	const FPrimaryAssetId	AssetId(AssetType, FName(*AssetName));

	LoadDataAssetAsync(AssetId, OnLoaded);
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

void UDataAssetManagerSubsystem::LoadAllDataAssetsOfTypeAsync(const TSubclassOf<UDataAsset>& AssetClass, const TFunction<void(UDataAsset*, bool)>& OnEachLoaded)
{
	if (!AssetClass)
	{
		return;
	}

	for (TArray<FPrimaryAssetId> AssetIds = GetAllAssetIds(AssetClass); const FPrimaryAssetId& AssetId : AssetIds)
	{
		LoadDataAssetAsync(AssetId, OnEachLoaded);
	}
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
	AssetManager->LoadPrimaryAssets(AssetIds);

	// Cache loaded assets
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

void UDataAssetManagerSubsystem::HandleAssetLoadedFunction(const FPrimaryAssetId& AssetId, const TFunction<void(UDataAsset*, bool)>& OnLoaded)
{
	// Remove from async handles
	AsyncHandles.Remove(AssetId);

	// Get the loaded asset
	UObject* Asset = AssetManager->GetPrimaryAssetObject(AssetId);
	if (UDataAsset* DataAsset = Cast<UDataAsset>(Asset))
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

bool UDataAssetManagerSubsystem::IsAssetCached(const FPrimaryAssetId& AssetId, UDataAsset*& OutCachedDataAsset) const
{
	if (const TObjectPtr<UDataAsset>* CachedAsset = LoadedAssets.Find(AssetId))
	{
		if (IsValid(CachedAsset->Get()))
		{
			UE_LOG(LogDataAssetManagerSubsystem, Log, TEXT("DataAssetManagerSubsystem: Loaded asset from cache: %s "), *AssetId.ToString());

			OutCachedDataAsset = CachedAsset->Get();
			return true;
		}
	}

	return false;
}

#pragma once

#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncFlowTask.h"

#include "DataAssetManagerSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDataAssetManagerSubsystem, Log, All);

/** Blueprint-compatible delegate for asset loading completion events */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataAssetLoaded, UDataAsset*, LoadedAsset, bool, bSuccess);

/**
 * Centralized subsystem for managing Primary Data Assets throughout the game.
 * Provides synchronous and asynchronous loading, caching, and registry functionality.
 * Async APIs return AsyncFlow::TTask<T> coroutines.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UDataAssetManagerSubsystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Load a data asset synchronously using its Primary Asset ID.
	 * @param AssetId The unique identifier for the asset to load
	 * @return Pointer to the loaded asset, or nullptr if loading failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	UDataAsset* LoadDataAssetByPrimaryAssetId(FPrimaryAssetId AssetId);

	/**
	 * Load a data asset synchronously by class type and name.
	 * @param AssetClass The class type of the asset to load
	 * @param AssetName The name/identifier of the specific asset
	 * @return Pointer to the loaded asset, or nullptr if loading failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager", meta = (DeterminesOutputType = "AssetClass"))
	UDataAsset* LoadDataAssetByClass(TSubclassOf<UDataAsset> AssetClass, const FString& AssetName);

	/**
	 * Load a data asset asynchronously using its Primary Asset ID.
	 * @param AssetId The unique identifier for the asset to load
	 * @return TTask that resolves to the loaded UDataAsset* (nullptr on failure)
	 */
	AsyncFlow::TTask<UDataAsset*> LoadDataAssetAsyncTask(FPrimaryAssetId AssetId);

	/**
	 * Load a data asset asynchronously by class and name.
	 * @param AssetClass The class type of the asset to load
	 * @param AssetName The name of the asset to load
	 * @return TTask that resolves to the loaded UDataAsset* (nullptr on failure)
	 */
	AsyncFlow::TTask<UDataAsset*> LoadDataAssetByClassAsyncTask(TSubclassOf<UDataAsset> AssetClass, const FString& AssetName);

	/**
	 * Load all assets of a specific type asynchronously.
	 * @param AssetClass The class type of assets to load
	 * @return TTask that resolves to an array of loaded UDataAsset*
	 */
	AsyncFlow::TTask<TArray<UDataAsset*>> LoadAllDataAssetsOfTypeAsyncTask(TSubclassOf<UDataAsset> AssetClass);

	/**
	 * Load all data assets of a specific type synchronously.
	 * @param AssetClass The class type of assets to load
	 * @return Array of all loaded assets of the specified type
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<UDataAsset*> LoadAllDataAssetsOfType(TSubclassOf<UDataAsset> AssetClass);

	/**
	 * Get all registered Primary Asset IDs in the system.
	 * @param AssetClass The class to query
	 * @return Array of all available asset IDs
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<FPrimaryAssetId> GetAllAssetIds(const TSubclassOf<UDataAsset> AssetClass) const;

	/**
	 * Check if a specific asset is currently loaded in memory.
	 * @param AssetId The asset ID to check
	 * @return True if the asset is loaded, false otherwise
	 */
	UFUNCTION(BlueprintPure, Category = "Data Asset Manager")
	bool IsAssetLoaded(const FPrimaryAssetId& AssetId) const;

	/**
	 * Preload multiple assets to improve runtime performance.
	 * @param AssetIds Array of asset IDs to preload
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	void PreloadAssets(const TArray<FPrimaryAssetId>& AssetIds);

	/**
	 * Unload a specific asset from memory cache.
	 * @param AssetId The asset to unload from cache
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	void UnloadAsset(FPrimaryAssetId AssetId);

	/** Clear all cached assets from memory */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	void ClearCache();

	/** Blueprint-bindable event fired when any asset finishes loading */
	UPROPERTY(BlueprintAssignable)
	FOnDataAssetLoaded OnDataAssetLoaded;

protected:
	bool IsAssetCached(const FPrimaryAssetId& AssetId, UDataAsset*& OutCachedDataAsset) const;

	UPROPERTY(Transient)
	TObjectPtr<UAssetManager> AssetManager;

	/** Cache of loaded assets for quick access */
	UPROPERTY(Transient)
	TMap<FPrimaryAssetId, TObjectPtr<UDataAsset>> LoadedAssets;

	/** Active async load tasks, keyed by asset ID for cancellation on teardown */
	TMap<FPrimaryAssetId, AsyncFlow::TTask<UDataAsset*>> ActiveLoadTasks;
};


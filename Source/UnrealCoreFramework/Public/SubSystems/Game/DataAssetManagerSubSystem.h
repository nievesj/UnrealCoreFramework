#pragma once

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "DataAssetManagerSubSystem.generated.h"

/** Log category for debugging and monitoring data asset operations */
DECLARE_LOG_CATEGORY_EXTERN(LogDataAssetManagerSubsystem, Log, All);

/** Blueprint-compatible delegate for asset loading completion events */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataAssetLoaded, UDataAsset*, LoadedAsset, bool, bSuccess);

/** TFunction callback type for C++ usage */
DECLARE_DELEGATE_TwoParams(FOnDataAssetLoadedDelegate, UDataAsset*, bool);

/**
 * Centralized subsystem for managing Primary Data Assets throughout the game.
 * Provides synchronous and asynchronous loading, caching, and registry functionality.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UDataAssetManagerSubsystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** Initialize the subsystem when the game instance starts */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Clean up resources when the subsystem shuts down */
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
	 * @param OnLoaded Callback function called when loading completes
	 */
	void LoadDataAssetAsync(FPrimaryAssetId AssetId, TFunction<void(UDataAsset*, bool)> OnLoaded);

	/**
	 * Load a data asset asynchronously by class and name.
	 * @param AssetClass The class type of the asset to load
	 * @param AssetName The name of the asset to load
	 * @param OnLoaded Callback function called when loading completes
	 */
	void LoadDataAssetByClassAsync(const TSubclassOf<UDataAsset>& AssetClass, const FString& AssetName, const TFunction<void(UDataAsset*, bool)>& OnLoaded);

	/**
	 * Load all assets of a specific type asynchronously.
	 * @param AssetClass The class type of assets to load
	 * @param OnEachLoaded Callback called for each asset as it finishes loading
	 */
	void LoadAllDataAssetsOfTypeAsync(const TSubclassOf<UDataAsset>& AssetClass, const TFunction<void(UDataAsset*, bool)>& OnEachLoaded);

	/**
	 * Load all data assets of a specific type synchronously.
	 * @param AssetClass The class type of assets to load
	 * @return Array of all loaded assets of the specified type
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<UDataAsset*> LoadAllDataAssetsOfType(TSubclassOf<UDataAsset> AssetClass);

	/**
	 * Get all registered Primary Asset IDs in the system.
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
	/**
	 * Internal callback handler for async loading operations.
	 * @param AssetId The ID of the asset that finished loading
	 * @param OnLoaded The user callback to execute
	 */
	void HandleAssetLoadedFunction(const FPrimaryAssetId& AssetId, const TFunction<void(UDataAsset*, bool)>& OnLoaded);

	bool IsAssetCached(const FPrimaryAssetId& AssetId, UDataAsset*& OutCachedDataAsset) const;

	UPROPERTY(Transient)
	TObjectPtr<UAssetManager> AssetManager;

	/** Cache of loaded assets for quick access */
	UPROPERTY(Transient)
	TMap<FPrimaryAssetId, TObjectPtr<UDataAsset>> LoadedAssets;

	/** Handles for ongoing async loading operations */
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> AsyncHandles;
};
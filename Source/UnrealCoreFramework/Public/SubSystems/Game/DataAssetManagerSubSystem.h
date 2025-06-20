#pragma once

#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "DataAssetManagerSubSystem.generated.h"

/** Log category for debugging and monitoring data asset operations */
DECLARE_LOG_CATEGORY_EXTERN(LogDataAssetManagerSubsystem, Log, All);

/** Blueprint-compatible delegate for asset loading completion events */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDataAssetLoaded, UPrimaryDataAsset*, LoadedAsset, bool, bSuccess);

/** TFunction callback type for C++ usage */
DECLARE_DELEGATE_TwoParams(FOnDataAssetLoadedDelegate, UPrimaryDataAsset*, bool);

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
	UPrimaryDataAsset* LoadDataAssetById(FPrimaryAssetId AssetId);

	/**
	 * Load a data asset synchronously by class type and name.
	 * @param AssetClass The class type of the asset to load
	 * @param AssetName The name/identifier of the specific asset
	 * @return Pointer to the loaded asset, or nullptr if loading failed
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager", meta = (DeterminesOutputType = "AssetClass"))
	UPrimaryDataAsset* LoadDataAssetByClass(TSubclassOf<UPrimaryDataAsset> AssetClass, const FString& AssetName);

	/**
	 * Template function for type-safe synchronous loading in C++.
	 * @param AssetName The name of the asset to load
	 * @return Pointer to the loaded asset cast to type T, or nullptr if loading failed
	 */
	template <typename T>
	T* LoadDataAsset(const FString& AssetName)
	{
		static_assert(TIsDerivedFrom<T, UPrimaryDataAsset>::IsDerived, "T must be derived from UPrimaryDataAsset");
		return Cast<T>(LoadDataAssetByClass(T::StaticClass(), AssetName));
	}

	/**
	 * Load a data asset asynchronously using its Primary Asset ID.
	 * @param AssetId The unique identifier for the asset to load
	 * @param OnLoaded Callback function called when loading completes
	 */
	void LoadDataAssetAsync(FPrimaryAssetId AssetId, TFunction<void(UPrimaryDataAsset*, bool)> OnLoaded);

	/**
	 * Load a data asset asynchronously by class and name.
	 * @param AssetClass The class type of the asset to load
	 * @param AssetName The name of the asset to load
	 * @param OnLoaded Callback function called when loading completes
	 */
	void LoadDataAssetByClassAsync(const TSubclassOf<UPrimaryDataAsset>& AssetClass, const FString& AssetName, const TFunction<void(UPrimaryDataAsset*, bool)>& OnLoaded);

	/**
	 * Load all assets of a specific type asynchronously.
	 * @param AssetClass The class type of assets to load
	 * @param OnEachLoaded Callback called for each asset as it finishes loading
	 */
	void LoadAllDataAssetsOfTypeAsync(const TSubclassOf<UPrimaryDataAsset>& AssetClass, const TFunction<void(UPrimaryDataAsset*, bool)>& OnEachLoaded);

	/**
	 * Load all data assets of a specific type synchronously.
	 * @param AssetClass The class type of assets to load
	 * @return Array of all loaded assets of the specified type
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<UPrimaryDataAsset*> LoadAllDataAssetsOfType(TSubclassOf<UPrimaryDataAsset> AssetClass);

	/**
	 * Get all registered Primary Asset IDs in the system.
	 * @return Array of all available asset IDs
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<FPrimaryAssetId> GetAllAssetIds() const;

	/**
	 * Get all asset IDs of a specific type.
	 * @param AssetType The type of assets to query
	 * @return Array of asset IDs matching the specified type
	 */
	UFUNCTION(BlueprintCallable, Category = "Data Asset Manager")
	TArray<FPrimaryAssetId> GetAssetIdsByType(FPrimaryAssetType AssetType) const;

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
	void HandleAssetLoadedFunction(const FPrimaryAssetId& AssetId, const TFunction<void(UPrimaryDataAsset*, bool)>& OnLoaded);

	/** Cache of loaded assets for quick access */
	UPROPERTY(Transient)
	TMap<FPrimaryAssetId, TObjectPtr<UPrimaryDataAsset>> LoadedAssets;

	/** Handles for ongoing async loading operations */
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> AsyncHandles;
};
#pragma once

#include "LevelManagerSubsystem.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadingStarted, FString, LevelName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadingComplete, FString, LevelName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelLoadingError, FString, LevelName, FString, ErrorMessage);

// Define callback type for lambda
DECLARE_DELEGATE_OneParam(FLevelLoadingCallback, bool);

UENUM(BlueprintType)
enum class ELevelLoadMethod : uint8
{
	Synchronous UMETA(DisplayName = "Synchronous"),
	Async		UMETA(DisplayName = "Asynchronous")
};

UENUM(BlueprintType)
enum class ELevelOperation : uint8
{
	Open   UMETA(DisplayName = "Open (Switch Level)"),
	Stream UMETA(DisplayName = "Stream (Add Level)")
};

UCLASS()
class UNREALCOREFRAMEWORK_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Initialize and Deinitialize
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Loads a level with the specified operation mode
	 * @param LevelPath - Path to the level
	 * @param Operation - Whether to open the level (switching to it) or stream it (adding to current world)
	 * @param LoadMethod - Synchronous or Asynchronous loading
	 */
	UFUNCTION(BlueprintCallable, Category = "Level Management", meta = (AutoCreateRefTerm = "Callback"))
	void LoadLevel(const FString& LevelPath,
		ELevelOperation			  Operation = ELevelOperation::Open,
		ELevelLoadMethod		  LoadMethod = ELevelLoadMethod::Async);

	/**
	 * Loads a level using soft object reference
	 * @param Level - Soft reference to the level
	 * @param Operation - Whether to open the level (switching to it) or stream it (adding to current world)
	 * @param LoadMethod - Synchronous or Asynchronous loading
	 */
	UFUNCTION(BlueprintCallable, Category = "Level Management", meta = (AutoCreateRefTerm = "Callback"))
	void LoadLevelSoftObject(const TSoftObjectPtr<UWorld>& Level,
		ELevelOperation									   Operation = ELevelOperation::Open,
		ELevelLoadMethod								   LoadMethod = ELevelLoadMethod::Async);

	// C++ versions with callback support
	void LoadLevel(const FString&	 LevelPath,
		const FLevelLoadingCallback& Callback,
		ELevelOperation				 Operation = ELevelOperation::Open,
		ELevelLoadMethod			 LoadMethod = ELevelLoadMethod::Async);

	void LoadLevel(const TSoftObjectPtr<UWorld>& Level,
		const FLevelLoadingCallback&			 Callback,
		ELevelOperation							 Operation = ELevelOperation::Open,
		ELevelLoadMethod						 LoadMethod = ELevelLoadMethod::Async);

	// Unload level
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void UnloadLevel(const FString& LevelPath);

	// Check if level is loaded
	UFUNCTION(BlueprintPure, Category = "Level Management")
	bool IsLevelLoaded(const FString& LevelPath) const;

	// Check if level is currently loading
	UFUNCTION(BlueprintPure, Category = "Level Management")
	bool IsLevelLoading(const FString& LevelPath) const;

	// Events
	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingStarted OnLevelLoadingStarted;

	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingComplete OnLevelLoadingComplete;

	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingError OnLevelLoadingError;

	UFUNCTION()
	void HandleLoadingComplete(const FString& LevelPath);

private:
	// Helper function to handle level loading completion

private:
	FDelegateHandle LevelLoadHandle;

	UFUNCTION()
	void OnLevelOpenComplete(UWorld* World);

	FString PendingLevelPath;

	// Map to track loading states
	TMap<FString, bool> LoadingLevels;

	// Validate level path
	static bool ValidateLevelPath(const FString& LevelPath);

	// Map to store callbacks
	TMap<FString, FLevelLoadingCallback> LoadingCallbacks;
};

UCLASS()
class ULevelLoadingCallbackHelper : public UObject
{
	GENERATED_BODY()

public:
	FLevelLoadingCallback Callback;
	FString				  LevelPath;

	UPROPERTY(Transient)
	ULevelManagerSubsystem* LevelManager;

	UFUNCTION()
	void OnLevelLoaded() const
	{
		if (IsValid(LevelManager))
		{
			// Let the manager handle the callback
			LevelManager->HandleLoadingComplete(LevelPath);
		}
		else if (Callback.IsBound())
		{
			// Fallback if manager is gone
			Callback.Execute(true);
		}
	}
};
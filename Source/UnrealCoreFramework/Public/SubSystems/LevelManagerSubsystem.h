#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "AsyncFlowTask.h"

#include "LevelManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadingStarted, FString, LevelName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelLoadingComplete, FString, LevelName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelLoadingError, FString, LevelName, FString, ErrorMessage);

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

/**
 * Subsystem for managing level loading and unloading operations.
 * Async APIs return AsyncFlow::TTask<bool> coroutines.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API ULevelManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/**
	 * Load a level by path (Blueprint entry point).
	 * Internally launches a coroutine task.
	 * @param LevelPath Path to the level
	 * @param Operation Open (full map travel) or Stream (additive)
	 * @param LoadMethod Synchronous or Asynchronous
	 */
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void LoadLevel(const FString& LevelPath,
		ELevelOperation Operation = ELevelOperation::Open,
		ELevelLoadMethod LoadMethod = ELevelLoadMethod::Async);

	/**
	 * Load a level via soft object reference (Blueprint entry point).
	 * @param Level Soft reference to the level
	 * @param Operation Open or Stream
	 * @param LoadMethod Synchronous or Asynchronous
	 */
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void LoadLevelSoftObject(const TSoftObjectPtr<UWorld>& Level,
		ELevelOperation Operation = ELevelOperation::Open,
		ELevelLoadMethod LoadMethod = ELevelLoadMethod::Async);

	/**
	 * Load a level asynchronously via coroutine.
	 * @param LevelPath Path to the level
	 * @param Operation Open or Stream
	 * @return TTask<bool> that resolves to true on success
	 */
	AsyncFlow::TTask<bool> LoadLevelTask(const FString& LevelPath, ELevelOperation Operation = ELevelOperation::Open);

	/**
	 * Load a level asynchronously via coroutine (soft object variant).
	 * @param Level Soft reference to the level
	 * @param Operation Open or Stream
	 * @return TTask<bool> that resolves to true on success
	 */
	AsyncFlow::TTask<bool> LoadLevelTask(const TSoftObjectPtr<UWorld>& Level, ELevelOperation Operation = ELevelOperation::Open);

	/**
	 * Unload a streaming level asynchronously via coroutine.
	 * @param LevelPath Path to the streaming level to unload
	 * @return TTask<bool> that resolves to true on success
	 */
	AsyncFlow::TTask<bool> UnloadLevelTask(const FString& LevelPath);

	/** Unload a level (Blueprint entry point, fire-and-forget). */
	UFUNCTION(BlueprintCallable, Category = "Level Management")
	void UnloadLevel(const FString& LevelPath);

	UFUNCTION(BlueprintPure, Category = "Level Management")
	bool IsLevelLoaded(const FString& LevelPath) const;

	UFUNCTION(BlueprintPure, Category = "Level Management")
	bool IsLevelLoading(const FString& LevelPath) const;

	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingStarted OnLevelLoadingStarted;

	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingComplete OnLevelLoadingComplete;

	UPROPERTY(BlueprintAssignable, Category = "Level Management|Events")
	FOnLevelLoadingError OnLevelLoadingError;

private:
	/** Map to track loading states */
	TMap<FString, bool> LoadingLevels;

	/** Active level load/unload tasks for concurrent operations and cancellation */
	TMap<FString, AsyncFlow::TTask<bool>> ActiveLevelTasks;

	static bool ValidateLevelPath(const FString& LevelPath);
};
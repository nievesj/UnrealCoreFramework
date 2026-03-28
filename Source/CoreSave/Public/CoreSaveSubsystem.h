// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreSaveSubsystem.generated.h"

class UCoreFrameworkSaveGame;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreSave, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSaveCompleted, const FString&, SlotName, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoadCompleted, const FString&, SlotName, bool, bSuccess);

/**
 * Subsystem managing save/load operations using UCoreFrameworkSaveGame.
 */
UCLASS()
class CORESAVE_API UCoreSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Synchronously save the current save game to the given slot. */
	UFUNCTION(BlueprintCallable, Category = "CoreSave")
	bool SaveGame(const FString& SlotName, int32 UserIndex = 0);

	/** Synchronously load a save game from the given slot. */
	UFUNCTION(BlueprintCallable, Category = "CoreSave")
	bool LoadGame(const FString& SlotName, int32 UserIndex = 0);

	/** Delete a save file from the given slot. */
	UFUNCTION(BlueprintCallable, Category = "CoreSave")
	bool DeleteSave(const FString& SlotName, int32 UserIndex = 0);

	/** Check whether a save file exists in the given slot. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CoreSave")
	bool DoesSaveExist(const FString& SlotName, int32 UserIndex = 0) const;

	/** Get the currently loaded/active save game object. May be null. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CoreSave")
	UCoreFrameworkSaveGame* GetCurrentSaveGame() const;

	/** Get the active save game or create a new one if none is loaded for this slot. */
	UFUNCTION(BlueprintCallable, Category = "CoreSave")
	UCoreFrameworkSaveGame* GetOrCreateSaveGame(const FString& SlotName, int32 UserIndex = 0);

	/** Return all known save slot names that have been saved or loaded through this subsystem. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CoreSave")
	TArray<FString> GetAllSaveSlotNames() const;

	/** Broadcast after a save operation completes. */
	UPROPERTY(BlueprintAssignable, Category = "CoreSave")
	FOnSaveCompleted OnSaveCompleted;

	/** Broadcast after a load operation completes. */
	UPROPERTY(BlueprintAssignable, Category = "CoreSave")
	FOnLoadCompleted OnLoadCompleted;

private:
	UPROPERTY()
	TObjectPtr<UCoreFrameworkSaveGame> CurrentSaveGame;

	UPROPERTY()
	TArray<FString> KnownSlotNames;
};
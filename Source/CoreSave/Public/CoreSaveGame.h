// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CoreSaveGame.generated.h"

/**
 * Generic save game object that stores arbitrary string and binary data.
 */
UCLASS(BlueprintType)
class CORESAVE_API UCoreFrameworkSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UCoreFrameworkSaveGame();

	/** The slot name this save was written to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGame")
	FString SaveSlotName;

	/** The user/controller index that owns this save. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGame")
	int32 UserIndex;

	/** Timestamp of when this save was last written. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGame")
	FDateTime SaveTimestamp;

	/** Generic string key-value data store. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGame")
	TMap<FString, FString> StringData;

	/** Generic binary data store keyed by string. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	TMap<FString, TArray<uint8>> BinaryData;

	/** Set a string value by key. */
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SetStringValue(const FString& Key, const FString& Value);

	/** Get a string value by key, returning DefaultValue if not found. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame")
	FString GetStringValue(const FString& Key, const FString& DefaultValue = TEXT("")) const;

	/** Check whether a key exists in the string data map. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame")
	bool HasKey(const FString& Key) const;

	/** Remove a key from both string and binary data maps. */
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void RemoveKey(const FString& Key);

	/** Store serialized binary data by key. */
	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void SetSerializedData(const FString& Key, const TArray<uint8>& Data);

	/** Retrieve serialized binary data by key. Returns empty array if not found. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SaveGame")
	TArray<uint8> GetSerializedData(const FString& Key) const;
};
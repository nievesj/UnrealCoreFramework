// MIT License
//
// Copyright (c) 2026 José M. Nieves
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "CoreSaveGame.generated.h"

/** Wrapper struct for binary blobs stored inside UCoreFrameworkSaveGame. */
USTRUCT(BlueprintType)
struct CORESAVE_API FCoreSaveByteArray
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveGame")
	TArray<uint8> Data;
};

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
	TMap<FString, FCoreSaveByteArray> BinaryData;

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
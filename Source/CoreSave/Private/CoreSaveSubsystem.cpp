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

#include "CoreSaveSubsystem.h"
#include "CoreSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Async/CoreAsyncTypes.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"

DEFINE_LOG_CATEGORY(LogCoreSave);

// ---------------------------------------------------------------------------
// UCoreFrameworkSaveGame implementation
// ---------------------------------------------------------------------------

UCoreFrameworkSaveGame::UCoreFrameworkSaveGame()
	: SaveSlotName(TEXT("Default"))
	, UserIndex(0)
	, SaveTimestamp(FDateTime::MinValue())
{
}

void UCoreFrameworkSaveGame::SetStringValue(const FString& Key, const FString& Value)
{
	StringData.Add(Key, Value);
}

FString UCoreFrameworkSaveGame::GetStringValue(const FString& Key, const FString& DefaultValue) const
{
	const FString* Found = StringData.Find(Key);
	return Found ? *Found : DefaultValue;
}

bool UCoreFrameworkSaveGame::HasKey(const FString& Key) const
{
	return StringData.Contains(Key);
}

void UCoreFrameworkSaveGame::RemoveKey(const FString& Key)
{
	StringData.Remove(Key);
	BinaryData.Remove(Key);
}

void UCoreFrameworkSaveGame::SetSerializedData(const FString& Key, const TArray<uint8>& Data)
{
	FCoreSaveByteArray& Entry = BinaryData.FindOrAdd(Key);
	Entry.Data = Data;
}

TArray<uint8> UCoreFrameworkSaveGame::GetSerializedData(const FString& Key) const
{
	const FCoreSaveByteArray* Found = BinaryData.Find(Key);
	return Found ? Found->Data : TArray<uint8>();
}

// ---------------------------------------------------------------------------
// UCoreSaveSubsystem implementation
// ---------------------------------------------------------------------------

void UCoreSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogCoreSave, Log, TEXT("CoreSaveSubsystem initialized."));
}

void UCoreSaveSubsystem::Deinitialize()
{
	CurrentSaveGame = nullptr;
	KnownSlotNames.Empty();
	UE_LOG(LogCoreSave, Log, TEXT("CoreSaveSubsystem deinitialized."));
	Super::Deinitialize();
}

bool UCoreSaveSubsystem::SaveGame(const FString& SlotName, int32 UserIndex)
{
	if (!CurrentSaveGame)
	{
		UE_LOG(LogCoreSave, Warning, TEXT("SaveGame: No active save game to save for slot '%s'."), *SlotName);
		OnSaveCompleted.Broadcast(SlotName, false);
		return false;
	}

	CurrentSaveGame->SaveSlotName = SlotName;
	CurrentSaveGame->UserIndex = UserIndex;
	CurrentSaveGame->SaveTimestamp = FDateTime::UtcNow();

	const bool bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, UserIndex);

	if (bSuccess)
	{
		if (!KnownSlotNames.Contains(SlotName))
		{
			KnownSlotNames.Add(SlotName);
		}
		UE_LOG(LogCoreSave, Log, TEXT("SaveGame: Successfully saved slot '%s'."), *SlotName);
	}
	else
	{
		UE_LOG(LogCoreSave, Error, TEXT("SaveGame: Failed to save slot '%s'."), *SlotName);
	}

	OnSaveCompleted.Broadcast(SlotName, bSuccess);
	return bSuccess;
}

bool UCoreSaveSubsystem::LoadGame(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UE_LOG(LogCoreSave, Warning, TEXT("LoadGame: Save slot '%s' does not exist."), *SlotName);
		OnLoadCompleted.Broadcast(SlotName, false);
		return false;
	}

	USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex);
	UCoreFrameworkSaveGame* TypedSave = Cast<UCoreFrameworkSaveGame>(LoadedGame);

	if (!TypedSave)
	{
		UE_LOG(LogCoreSave, Error, TEXT("LoadGame: Failed to load or cast save from slot '%s'."), *SlotName);
		OnLoadCompleted.Broadcast(SlotName, false);
		return false;
	}

	CurrentSaveGame = TypedSave;

	if (!KnownSlotNames.Contains(SlotName))
	{
		KnownSlotNames.Add(SlotName);
	}

	UE_LOG(LogCoreSave, Log, TEXT("LoadGame: Successfully loaded slot '%s'."), *SlotName);
	OnLoadCompleted.Broadcast(SlotName, true);
	return true;
}

bool UCoreSaveSubsystem::DeleteSave(const FString& SlotName, int32 UserIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UE_LOG(LogCoreSave, Warning, TEXT("DeleteSave: Save slot '%s' does not exist."), *SlotName);
		return false;
	}

	const bool bDeleted = UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);

	if (bDeleted)
	{
		KnownSlotNames.Remove(SlotName);

		if (CurrentSaveGame && CurrentSaveGame->SaveSlotName == SlotName)
		{
			CurrentSaveGame = nullptr;
		}

		UE_LOG(LogCoreSave, Log, TEXT("DeleteSave: Deleted slot '%s'."), *SlotName);
	}
	else
	{
		UE_LOG(LogCoreSave, Error, TEXT("DeleteSave: Failed to delete slot '%s'."), *SlotName);
	}

	return bDeleted;
}

bool UCoreSaveSubsystem::DoesSaveExist(const FString& SlotName, int32 UserIndex) const
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

UCoreFrameworkSaveGame* UCoreSaveSubsystem::GetCurrentSaveGame() const
{
	return CurrentSaveGame;
}

UCoreFrameworkSaveGame* UCoreSaveSubsystem::GetOrCreateSaveGame(const FString& SlotName, int32 UserIndex)
{
	// If we already have one loaded for this slot, return it.
	if (CurrentSaveGame && CurrentSaveGame->SaveSlotName == SlotName)
	{
		return CurrentSaveGame;
	}

	// Try loading from disk.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		if (LoadGame(SlotName, UserIndex))
		{
			return CurrentSaveGame;
		}
	}

	// Create a brand new save game.
	CurrentSaveGame = Cast<UCoreFrameworkSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UCoreFrameworkSaveGame::StaticClass()));
	CurrentSaveGame->SaveSlotName = SlotName;
	CurrentSaveGame->UserIndex = UserIndex;

	if (!KnownSlotNames.Contains(SlotName))
	{
		KnownSlotNames.Add(SlotName);
	}

	UE_LOG(LogCoreSave, Log, TEXT("GetOrCreateSaveGame: Created new save game for slot '%s'."), *SlotName);
	return CurrentSaveGame;
}

TArray<FString> UCoreSaveSubsystem::GetAllSaveSlotNames() const
{
	return KnownSlotNames;
}

AsyncFlow::TTask<bool> UCoreSaveSubsystem::SaveGameAsync(const FString& SlotName, int32 UserIndex)
{
	UCF_ASYNC_CONTRACT(this);

	if (!CurrentSaveGame)
	{
		UE_LOG(LogCoreSave, Warning, TEXT("SaveGameAsync: No active save game for slot '%s'."), *SlotName);
		OnSaveCompleted.Broadcast(SlotName, false);
		co_return false;
	}

	CurrentSaveGame->SaveSlotName = SlotName;
	CurrentSaveGame->UserIndex = UserIndex;
	CurrentSaveGame->SaveTimestamp = FDateTime::UtcNow();

	FAsyncSaveGameToSlotDelegate OnComplete;
	bool bAsyncResult = false;

	OnComplete.BindWeakLambda(this, [this, SlotName, &bAsyncResult](const FString& InSlotName, const int32 InUserIndex, bool bSuccess)
	{
		bAsyncResult = bSuccess;
		if (bSuccess)
		{
			if (!KnownSlotNames.Contains(InSlotName))
			{
				KnownSlotNames.Add(InSlotName);
			}
			UE_LOG(LogCoreSave, Log, TEXT("SaveGameAsync: Saved slot '%s'."), *InSlotName);
		}
		else
		{
			UE_LOG(LogCoreSave, Error, TEXT("SaveGameAsync: Failed to save slot '%s'."), *InSlotName);
		}
		OnSaveCompleted.Broadcast(InSlotName, bSuccess);
	});

	UGameplayStatics::AsyncSaveGameToSlot(CurrentSaveGame, SlotName, UserIndex, OnComplete);

	// Yield one frame so the async callback fires
	co_await AsyncFlow::NextTick(this);

	co_return bAsyncResult;
}

AsyncFlow::TTask<bool> UCoreSaveSubsystem::LoadGameAsync(const FString& SlotName, int32 UserIndex)
{
	UCF_ASYNC_CONTRACT(this);

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UE_LOG(LogCoreSave, Warning, TEXT("LoadGameAsync: Slot '%s' does not exist."), *SlotName);
		OnLoadCompleted.Broadcast(SlotName, false);
		co_return false;
	}

	bool bAsyncResult = false;

	FAsyncLoadGameFromSlotDelegate OnComplete;
	OnComplete.BindWeakLambda(this, [this, SlotName, &bAsyncResult](const FString& InSlotName, const int32 InUserIndex, USaveGame* LoadedGame)
	{
		UCoreFrameworkSaveGame* TypedSave = Cast<UCoreFrameworkSaveGame>(LoadedGame);
		if (TypedSave)
		{
			CurrentSaveGame = TypedSave;
			if (!KnownSlotNames.Contains(InSlotName))
			{
				KnownSlotNames.Add(InSlotName);
			}
			UE_LOG(LogCoreSave, Log, TEXT("LoadGameAsync: Loaded slot '%s'."), *InSlotName);
			bAsyncResult = true;
		}
		else
		{
			UE_LOG(LogCoreSave, Error, TEXT("LoadGameAsync: Failed to load slot '%s'."), *InSlotName);
			bAsyncResult = false;
		}
		OnLoadCompleted.Broadcast(InSlotName, bAsyncResult);
	});

	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, UserIndex, OnComplete);

	co_await AsyncFlow::NextTick(this);

	co_return bAsyncResult;
}

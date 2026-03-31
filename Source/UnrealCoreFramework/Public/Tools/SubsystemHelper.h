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

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Subsystems/EngineSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "Subsystems/WorldSubsystem.h"

#include "SubsystemHelper.generated.h"

/**
 * A utility class for retrieving various subsystem instances based on their type and the provided context.
 * USubsystemHelper simplifies the process of retrieving subsystems associated with the given context.
 * Supports UGameInstanceSubsystem, UWorldSubsystem, UEngineSubsystem, and ULocalPlayerSubsystem types.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API USubsystemHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Get a subsystem of the specified type using the provided context object. Returns nullptr if subsystem cannot be found. */
	template <typename T>
	static T* GetSubsystem(const UObject* Context)
	{
		static_assert(TIsDerivedFrom<T, USubsystem>::Value, "T must inherit from USubsystem");

		// Engine subsystems don't need context validation
		if constexpr (TIsDerivedFrom<T, UEngineSubsystem>::Value)
		{
			return IsValid(GEngine) ? GEngine->GetEngineSubsystem<T>() : nullptr;
		}

		// All other subsystems need valid context
		if (!IsValid(Context) || !IsValid(GEngine))
		{
			return nullptr;
		}

		const UWorld* World = GEngine->GetWorldFromContextObject(Context, EGetWorldErrorMode::ReturnNull);
		if (!IsValid(World))
		{
			return nullptr;
		}

		// World subsystem
		if constexpr (TIsDerivedFrom<T, UWorldSubsystem>::Value)
		{
			return World->GetSubsystem<T>();
		}

		// Game Instance subsystem
		if constexpr (TIsDerivedFrom<T, UGameInstanceSubsystem>::Value)
		{
			const UGameInstance* GameInstance = World->GetGameInstance();
			return IsValid(GameInstance) ? GameInstance->GetSubsystem<T>() : nullptr;
		}

		// Local Player subsystem with multiple fallback strategies
		if constexpr (TIsDerivedFrom<T, ULocalPlayerSubsystem>::Value)
		{
			return GetLocalPlayerSubsystem<T>(World);
		}

		return nullptr;
	}

private:
	/** Helper method to get LocalPlayer subsystem with multiple fallback strategies */
	template <typename T>
	static T* GetLocalPlayerSubsystem(const UWorld* World)
	{
		if (!IsValid(World))
		{
			return nullptr;
		}

		const UGameInstance* GameInstance = World->GetGameInstance();
		if (!IsValid(GameInstance))
		{
			return nullptr;
		}

		// Strategy 1: Try first game player (most reliable)
		const ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer();
		if (IsValid(LocalPlayer))
		{
			return LocalPlayer->GetSubsystem<T>();
		}

		// Strategy 2: Try through first local player controller
		const APlayerController* PC = GameInstance->GetFirstLocalPlayerController();
		if (IsValid(PC))
		{
			LocalPlayer = PC->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				return LocalPlayer->GetSubsystem<T>();
			}
		}

		// Strategy 3: Iterate through all local players as last resort
		for (const ULocalPlayer* Player : GameInstance->GetLocalPlayers())
		{
			if (IsValid(Player))
			{
				return Player->GetSubsystem<T>();
			}
		}

		return nullptr;
	}
};
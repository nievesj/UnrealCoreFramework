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

#include "GameFramework/GameModeBase.h"

#include "CoreGameMode.generated.h"

/**
 * Framework base game mode. Sets up default Core Framework pawn and game state classes.
 * Override OnGameStateInitialized() for post-init logic.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API ACoreGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void InitGameState() override;

	/** Called after the game state has been created and initialized. */
	virtual void OnGameStateInitialized();

	/** Default pawn class to spawn for players. */
	UPROPERTY(EditAnywhere, Category = UnrealCoreFrameworkSettings)
	TSubclassOf<APawn> PlayerPawnClass;
};

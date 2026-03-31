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

#include "Game/Base/CoreGameInstance.h"

#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Game/Base/CoreGameMode.h"
#include "SubSystems/LocalPlayer/UISubsystem.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogCoreGameInstance);

UCoreGameInstance::UCoreGameInstance()
{
}

void UCoreGameInstance::Init()
{
	Super::Init();

	// OnLocalPlayerAddedEvent.AddUObject(this, &UCoreGameInstance::OnLocalPlayerAdded);
}

void UCoreGameInstance::OnStart()
{
	Super::OnStart();
	UE_VLOG_UELOG(this, LogCoreGameInstance, Log, TEXT("Initializing UI CoreGameInstance"));

	if (const APlayerController* PC = GetFirstLocalPlayerController())
	{
		if (const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
			{
				UISubsystem->CreateMainUIContainer();
				UE_VLOG_UELOG(this, LogCoreGameInstance, Log, TEXT("Created Main UI Container"));
			}
		}
	}
}

AGameModeBase* UCoreGameInstance::CreateGameModeForURL(FURL InURL, UWorld* InWorld)
{
	if (AGameModeBase* GameMode = Super::CreateGameModeForURL(InURL, InWorld))
	{
		return GameMode;
	}
	return nullptr;
}

void UCoreGameInstance::FinishDestroy()
{
	Super::FinishDestroy();
}

void UCoreGameInstance::Shutdown()
{
	Super::Shutdown();
	// OnLocalPlayerAddedEvent.RemoveAll(this);
}

#if WITH_EDITOR
FGameInstancePIEResult UCoreGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	FGameInstancePIEResult Result = Super::StartPlayInEditorGameInstance(LocalPlayer, Params);

	if (LocalPlayer)
	{
		if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
		{
			UISubsystem->CreateMainUIContainer();
			UE_VLOG_UELOG(this, LogCoreGameInstance, Log, TEXT("Created Main UI Container"));
		}
	}

	return Result;
}
#endif

void UCoreGameInstance::OnLocalPlayerAdded(ULocalPlayer* LocalPlayer)
{
}

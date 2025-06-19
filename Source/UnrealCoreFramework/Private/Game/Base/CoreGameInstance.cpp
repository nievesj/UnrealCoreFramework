// Fill out your copyright notice in the Description page of Project Settings.

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
			UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>();
			UISubsystem->CreateMainUIContainer();
			UE_VLOG_UELOG(this, LogCoreGameInstance, Log, TEXT("Created Main UI Container"));
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
		UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>();
		UISubsystem->CreateMainUIContainer();
		UE_VLOG_UELOG(this, LogCoreGameInstance, Log, TEXT("Created Main UI Container"));
	}

	return Result;
}
#endif

void UCoreGameInstance::OnLocalPlayerAdded(ULocalPlayer* LocalPlayer)
{
}

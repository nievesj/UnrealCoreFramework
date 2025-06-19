// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"

#include "CoreGameInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreGameInstance, Log, All);

UCLASS()
class UNREALCOREFRAMEWORK_API UCoreGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UCoreGameInstance();
	virtual class AGameModeBase* CreateGameModeForURL(FURL InURL, UWorld* InWorld) override;

	virtual void Init() override;
	virtual void OnStart() override;
	virtual void FinishDestroy() override;
	virtual void Shutdown() override;

#if WITH_EDITOR
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif

protected:
	void OnLocalPlayerAdded(ULocalPlayer* LocalPlayer);
};
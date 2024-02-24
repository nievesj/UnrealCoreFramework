﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "UI/CorePage.h"

#include "UnrealCoreFrameworkSettings.generated.h"

UCLASS(config = Engine, defaultconfig, meta = (DisplayName = "Unreal Core Framework"))
class UNREALCOREFRAMEWORK_API UUnrealCoreFrameworkSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UUnrealCoreFrameworkSettings* GetSettings()
	{
		return GetDefault<UUnrealCoreFrameworkSettings>();
	}

	UPROPERTY(EditAnywhere, config, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Main Menu Page"))
	TSubclassOf<UCorePage> MainMenuPage;

	UPROPERTY(EditAnywhere, config, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Main HUD Page"))
	TSubclassOf<UCorePage> MainHUDPage;

	UPROPERTY(EditAnywhere, config, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Pause Menu Page"))
	TSubclassOf<UCorePage> PauseMenuPage;
};

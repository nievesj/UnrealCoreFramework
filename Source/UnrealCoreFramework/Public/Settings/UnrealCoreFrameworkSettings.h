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

#include "Engine/DeveloperSettings.h"
#include "UI/CorePage.h"

#include "UnrealCoreFrameworkSettings.generated.h"

class UMainUiContainer;
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

	UPROPERTY(EditAnywhere, config, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Main UI Container"))
	TSubclassOf<UMainUiContainer> MainUIContainer;

	UPROPERTY(EditAnywhere, config, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Prompt"))
	TSubclassOf<UCoreWidget> YesNoPrompt;
};

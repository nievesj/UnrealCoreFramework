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

#include "CoreWidget.h"
#include "GameFramework/HUD.h"

#include "CoreHUD.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreHUD, Log, All);

/**
 * Framework HUD actor. Creates and manages the main HUD widget
 * from the class specified in MainPageClass or Core Framework settings.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API ACoreHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	/** Create and display the main HUD widget. */
	UFUNCTION(BlueprintCallable, Category = UnrealCoreFrameworkSettings)
	virtual void ShowMainHUD();

	/** Hide and remove the main HUD widget. */
	UFUNCTION(BlueprintCallable, Category = UnrealCoreFrameworkSettings)
	virtual void HideMainHUD();

protected:
	/** Widget class to instantiate as the main HUD page. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Main HUD Class"))
	TSubclassOf<UCoreWidget> MainPageClass;

	/** The active main HUD widget instance. */
	UPROPERTY(Transient)
	UCoreWidget* MainPage;
};
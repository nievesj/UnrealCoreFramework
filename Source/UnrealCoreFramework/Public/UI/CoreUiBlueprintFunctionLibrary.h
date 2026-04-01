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

#include "Kismet/BlueprintFunctionLibrary.h"

#include "CoreUiBlueprintFunctionLibrary.generated.h"

enum class EWidgetContainerType : uint8;
class UCorePage;
class UCoreWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreUiFunctionLibrary, Log, All);

/**
 Blueprint library for the Core Framework UI System
 */
UCLASS(meta = (ScriptName = "CoreUiFunctionLibrary"))
class UNREALCOREFRAMEWORK_API UCoreUiBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static UCoreWidget* CreateViewportPage(UCoreWidget* Widget, TSubclassOf<UCoreWidget> PageClass);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static void RemoveViewportPage(UCoreWidget* Widget, UCorePage* Page);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static bool AddWidgetToStack(UCoreWidget* Widget, const TSubclassOf<UCoreWidget> PageClass, const EWidgetContainerType StackContainerType, UCoreWidget*& OutCoreWidget);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi)
	static void RemoveWidgetFromStack(UCoreWidget* Widget, const EWidgetContainerType& StackContainerType, bool Destroy = false);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static void CreateMainHUD(UCoreWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static void CreateMainMenu(UCoreWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static void CreatePauseMenu(UCoreWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static bool GetMainPage(UCoreWidget* Widget, ECoreMainPageType CoreMainPageType, UCoreWidget*& OutCoreWidget);

	UFUNCTION(BlueprintCallable, Category = CoreFrameworkUi, Meta = (DefaultToSelf = "Widget"))
	static bool CreatePrompt(UCoreWidget* Widget, const FText& PromptText, int PromptIndex, UCoreWidget*& OutPromptWidget);
};

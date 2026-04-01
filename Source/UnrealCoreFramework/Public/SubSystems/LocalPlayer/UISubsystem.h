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

#include "SubSystems/Base/CoreLocalPlayerSubsystem.h"
#include "UI/PageableWidgetInterface.h"

#include "UISubsystem.generated.h"

enum class EWidgetContainerType : uint8;
class UMainUiContainer;
class UCorePage;
class ACorePlayerController;
class UCoreWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogUISubsystem, Log, All);

UENUM(BlueprintType)
enum class ECoreMainPageType : uint8
{
	MainMenu,
	MainHUD,
	PauseMenu
};

UCLASS()
class UNREALCOREFRAMEWORK_API UUISubsystem : public UCoreLocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UCoreWidget* CreateViewportPage(APlayerController* Owner, TSubclassOf<UCoreWidget> PageClass);
	void		 RemoveViewportPage(IPageableWidgetInterface* Page);
	void		 RemoveAllViewportPages();

	UCoreWidget* AddWidgetToStack(const TSubclassOf<UCoreWidget>& PageClass, const EWidgetContainerType& StackContainerType);
	void		 RemoveWidgetFromStack(UCoreWidget* Widget, const EWidgetContainerType& StackContainerType, bool Destroy = false);

	/** Navigate back to the previous page in history. */
	void GoBack();


	/** Creates the main UI container from settings. */
	void CreateMainUIContainer();

	/** Returns the top-most page in the open widget stack. */
	IPageableWidgetInterface* GetTopPage();

	UMainUiContainer* GetMainUIContainer() const
	{
		return MainUiContainer;
	};

	bool ShouldDisablePlayerControllerInput();
	void SetPlayerControllerInput(APlayerController* PC, bool bIsDisabled);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	UPROPERTY(Transient)
	TArray<UCoreWidget*> CoreWidgetsOpen;

	UPROPERTY(Transient)
	TArray<TSubclassOf<UCorePage>> PageHistory;

	UPROPERTY(Transient)
	UMainUiContainer* MainUiContainer;
};

// MIT License. Copyright (c) 2024  José M. Nieves

#include "UI/CoreUiBlueprintFunctionLibrary.h"

#include "Settings/UnrealCoreFrameworkSettings.h"
#include "SubSystems/LocalPlayer/UISubsystem.h"
#include "UI/CoreWidget.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogCoreUiFunctionLibrary);

UCoreWidget* UCoreUiBlueprintFunctionLibrary::CreateViewportPage(UCoreWidget* Widget, TSubclassOf<UCoreWidget> PageClass)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Widget is invalid"));
	}

	APlayerController* PC = Widget->GetOwningPlayer();
	if (!PC)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Failed to get Player Controller"));
	}

	/*UUISubsystem* UISubsystem = Widget->GetUISubsystem();
	if (!UISubsystem)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Failed to get UISubsystem"));
	}

	return UISubsystem->CreateViewportPage(PC, PageClass);*/

	return nullptr;
}

void UCoreUiBlueprintFunctionLibrary::RemoveViewportPage(UCoreWidget* Widget, UCorePage* Page)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::RemovePage - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		UISubsystem->RemoveViewportPage(Cast<IPageableWidgetInterface>(Page));
	}*/
}

bool UCoreUiBlueprintFunctionLibrary::AddWidgetToStack(UCoreWidget* Widget, const TSubclassOf<UCoreWidget> PageClass, const EWidgetContainerType StackContainerType, UCoreWidget*& OutCoreWidget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::RemovePage - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		OutCoreWidget = UISubsystem->AddWidgetToStack(PageClass, StackContainerType);
		return true;
	}*/
	return false;
}

void UCoreUiBlueprintFunctionLibrary::RemoveWidgetFromStack(UCoreWidget* Widget, const EWidgetContainerType& StackContainerType, bool Destroy)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::RemovePage - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		UISubsystem->RemoveWidgetFromStack(*Widget, StackContainerType, Destroy);
	}*/
}

void UCoreUiBlueprintFunctionLibrary::CreateMainHUD(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreateMainHUD - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		// UISubsystem->CreateMainPage(ECoreMainPageType::MainHUD);
	}*/
}

void UCoreUiBlueprintFunctionLibrary::CreateMainMenu(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreateMainMenu - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		// UISubsystem->CreateMainPage(ECoreMainPageType::MainMenu);
	}*/
}

void UCoreUiBlueprintFunctionLibrary::CreatePauseMenu(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePauseMenu - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		// UISubsystem->CreateMainPage(ECoreMainPageType::PauseMenu);
	}*/
}

bool UCoreUiBlueprintFunctionLibrary::GetMainPage(UCoreWidget* Widget, ECoreMainPageType CoreMainPageType, UCoreWidget*& OutCoreWidget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePauseMenu - Widget is invalid"));
	}

	/*if (UUISubsystem* UISubsystem = Widget->GetUISubsystem())
	{
		// OutCoreWidget = UISubsystem->GetMainPage(CoreMainPageType);
		return true;
	}*/

	return false;
}
bool UCoreUiBlueprintFunctionLibrary::CreatePrompt(UCoreWidget* Widget, const FText& PromptText, int PromptIndex, UCoreWidget*& OutPromptWidget)
{
	const UUnrealCoreFrameworkSettings* Settings = UUnrealCoreFrameworkSettings::GetSettings();
	if (!Settings)
	{
		return false;
	}

	return AddWidgetToStack(Widget, Settings->YesNoPrompt, EWidgetContainerType::Modal, OutPromptWidget);
}

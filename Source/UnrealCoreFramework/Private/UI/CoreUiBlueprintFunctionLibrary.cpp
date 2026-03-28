// MIT License. Copyright (c) 2024  José M. Nieves

#include "UI/CoreUiBlueprintFunctionLibrary.h"

#include "Engine/LocalPlayer.h"
#include "Settings/UnrealCoreFrameworkSettings.h"
#include "SubSystems/LocalPlayer/UISubsystem.h"
#include "UI/CorePage.h"
#include "UI/CoreWidget.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogCoreUiFunctionLibrary);

UCoreWidget* UCoreUiBlueprintFunctionLibrary::CreateViewportPage(UCoreWidget* Widget, TSubclassOf<UCoreWidget> PageClass)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Widget is invalid"));
		return nullptr;
	}

	APlayerController* PC = Widget->GetOwningPlayer();
	if (!PC)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Failed to get Player Controller"));
		return nullptr;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Failed to get Local Player"));
		return nullptr;
	}

	UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>();
	if (!UISubsystem)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePage - Failed to get UISubsystem"));
		return nullptr;
	}

	return UISubsystem->CreateViewportPage(PC, PageClass);
}

void UCoreUiBlueprintFunctionLibrary::RemoveViewportPage(UCoreWidget* Widget, UCorePage* Page)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::RemovePage - Widget is invalid"));
		return;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->RemoveViewportPage(Cast<IPageableWidgetInterface>(Page));
	}
}

bool UCoreUiBlueprintFunctionLibrary::AddWidgetToStack(UCoreWidget* Widget, const TSubclassOf<UCoreWidget> PageClass, const EWidgetContainerType StackContainerType, UCoreWidget*& OutCoreWidget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::AddWidgetToStack - Widget is invalid"));
		return false;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return false;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		OutCoreWidget = UISubsystem->AddWidgetToStack(PageClass, StackContainerType);
		return OutCoreWidget != nullptr;
	}

	return false;
}

void UCoreUiBlueprintFunctionLibrary::RemoveWidgetFromStack(UCoreWidget* Widget, const EWidgetContainerType& StackContainerType, bool Destroy)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::RemoveWidgetFromStack - Widget is invalid"));
		return;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->RemoveWidgetFromStack(*Widget, StackContainerType, Destroy);
	}
}

void UCoreUiBlueprintFunctionLibrary::CreateMainHUD(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreateMainHUD - Widget is invalid"));
		return;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->CreateMainUIContainer();
	}
}

void UCoreUiBlueprintFunctionLibrary::CreateMainMenu(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreateMainMenu - Widget is invalid"));
		return;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->CreateMainUIContainer();
	}
}

void UCoreUiBlueprintFunctionLibrary::CreatePauseMenu(UCoreWidget* Widget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::CreatePauseMenu - Widget is invalid"));
		return;
	}

	ULocalPlayer* LocalPlayer = Widget->GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	if (UUISubsystem* UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>())
	{
		UISubsystem->CreateMainUIContainer();
	}
}

bool UCoreUiBlueprintFunctionLibrary::GetMainPage(UCoreWidget* Widget, ECoreMainPageType CoreMainPageType, UCoreWidget*& OutCoreWidget)
{
	if (!Widget)
	{
		UE_VLOG_UELOG(Widget, LogCoreUiFunctionLibrary, Error, TEXT("UCoreUiBlueprintFunctionLibrary::GetMainPage - Widget is invalid"));
		return false;
	}

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

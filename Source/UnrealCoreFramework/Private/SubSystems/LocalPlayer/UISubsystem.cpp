#include "SubSystems/LocalPlayer/UISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "Settings/UnrealCoreFrameworkSettings.h"
#include "UI/CoreHUD.h"
#include "UI/MainUiContainer.h"
#include "UI/UiCoreFrameworkTypes.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogUISubsystem);

UCoreWidget* UUISubsystem::CreateViewportPage(APlayerController* Owner, TSubclassOf<UCoreWidget> PageClass)
{
	if (!Owner)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Owner is null"));
		return nullptr;
	}

	if (!IsValid(PageClass))
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("PageClass is not valid"));
		return nullptr;
	}

	if (const UClass* Class = PageClass.Get(); !Class->ImplementsInterface(UPageableWidgetInterface::StaticClass()))
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Parameter Page does not implement Interface UPageableWidgetInterface."));
		return nullptr;
	}

	if (UCoreWidget* Page = CreateWidget<UCoreWidget>(Owner, PageClass))
	{
		Page->AddToViewport();
		CoreWidgetsOpen.Push(Page);

		if (IPageableWidgetInterface* PageInt = Cast<IPageableWidgetInterface>(Page))
		{
			PageInt->Open();
		}

		UE_VLOG_UELOG(this, LogUISubsystem, Log, TEXT("Created Page %s"), *Page->GetName());
		return Page;
	}

	UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Failed to create Page."));
	return nullptr;
}

void UUISubsystem::RemoveViewportPage(IPageableWidgetInterface* Page)
{
	if (Page)
	{
		if (UCoreWidget* Widget = Cast<UCoreWidget>(Page))
		{
			CoreWidgetsOpen.Remove(Widget);

			if (APlayerController* PC = Widget->GetOwningPlayer())
			{
				SetPlayerControllerInput(PC, ShouldDisablePlayerControllerInput());
			}

			Widget->RemoveFromParent();
		}
	}
}

void UUISubsystem::RemoveAllViewportPages()
{
	for (UCoreWidget* Widget : CoreWidgetsOpen)
	{
		if (IPageableWidgetInterface* Page = Cast<IPageableWidgetInterface>(Widget))
		{
			Page->Close();
		}
	}

	CoreWidgetsOpen.Empty();
}

UCoreWidget* UUISubsystem::AddWidgetToStack(const TSubclassOf<UCoreWidget>& PageClass, const EWidgetContainerType& StackContainerType)
{
	if (!MainUiContainer)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("UUISubsystem::AddWidgetToStack - MainUiContainer is invalid"));
		return nullptr;
	}
	UCoreWidget* Widget = MainUiContainer->AddWidgetToStack(PageClass, StackContainerType);
	if (Widget)
	{
		Widget->Show();
	}
	return Widget;
}

void UUISubsystem::RemoveWidgetFromStack(UCoreWidget& Widget, const EWidgetContainerType& StackContainerType, bool Destroy)
{
	if (!MainUiContainer)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("UUISubsystem::RemoveWidgetFromStack - MainUiContainer is invalid"));
		return;
	}

	MainUiContainer->RemoveWidgetFromStack(Widget, StackContainerType);
}

IPageableWidgetInterface* UUISubsystem::GetTopPage()
{
	for (int32 i = CoreWidgetsOpen.Num() - 1; i >= 0; --i)
	{
		if (IPageableWidgetInterface* Page = Cast<IPageableWidgetInterface>(CoreWidgetsOpen[i]))
		{
			return Page;
		}
	}
	return nullptr;
}

void UUISubsystem::CreateMainUIContainer()
{
	const UUnrealCoreFrameworkSettings* Settings = UUnrealCoreFrameworkSettings::GetSettings();
	if (!Settings)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Failed to get Developer Settings for UnrealCoreFrameworkSettings."));
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Failed to get World."));
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Failed to get Player Controller."));
		return;
	}

	if (UCoreWidget* Widget = CreateViewportPage(PC, *Settings->MainUIContainer))
	{
		MainUiContainer = Cast<UMainUiContainer>(Widget);

		UCorePage* Page = MainUiContainer->AddWidgetToStack<UCorePage>(Settings->MainMenuPage, EWidgetContainerType::Page);
		if (Page)
		{
			Page->Open();
		}
	}
}

bool UUISubsystem::ShouldDisablePlayerControllerInput()
{
	for (UCoreWidget* Widget : CoreWidgetsOpen)
	{
		if (const IPageableWidgetInterface* Page = Cast<IPageableWidgetInterface>(Widget))
		{
			if (Page->GetDisablePlayerInput())
			{
				return true;
			}
		}
	}

	return false;
}

void UUISubsystem::SetPlayerControllerInput(APlayerController* PC, bool IsDisabled)
{
	if (!PC)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Player Controller is invalid"));
		return;
	}

	ACharacter* Character = PC->GetCharacter();
	if (!Character)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Error, TEXT("Character is invalid"));
		return;
	}

	IsDisabled ? Character->DisableInput(PC) : Character->EnableInput(PC);
}

void UUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_VLOG_UELOG(this, LogUISubsystem, Log, TEXT("Initializing UI Subsystem"));
}

void UUISubsystem::Deinitialize()
{
	UE_LOG(LogUISubsystem, Warning, TEXT("Deinitializing UI Subsystem"))
}

void UUISubsystem::GoBack()
{
	if (PageHistory.Num() == 0)
	{
		UE_VLOG_UELOG(this, LogUISubsystem, Warning, TEXT("GoBack: No pages in history."));
		return;
	}

	const TSubclassOf<UCorePage> PreviousPageClass = PageHistory.Pop();

	if (MainUiContainer)
	{
		AddWidgetToStack(PreviousPageClass, EWidgetContainerType::Page);
	}
}


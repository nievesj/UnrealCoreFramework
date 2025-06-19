#include "UI/CorePage.h"

#include "Components/Button.h"
#include "SubSystems/LocalPlayer/UISubsystem.h"

void UCorePage::NativeConstruct()
{
	Super::NativeConstruct();

	const UWorld* World = GetWorld();
	if (!IsValid(World))
		return;

	if (const APlayerController* PC = World->GetFirstPlayerController())
	{
		if (const ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			UISubsystem = LocalPlayer->GetSubsystem<UUISubsystem>();
		}
	}
}

void UCorePage::NativeDestruct()
{
	Super::NativeDestruct();
	DisablePlayerControllerInput = false;
}

void UCorePage::InternalShown()
{
	if (ExitButton)
	{
		ExitButton->OnClicked.AddDynamic(this, &UCorePage::Handle_OnExitButtonClicked);
	}
	Super::InternalShown();
}

void UCorePage::InternalHidden()
{
	Super::InternalHidden();
}

void UCorePage::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UCorePage::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	DisablePlayerControllerInput = false;
}

void UCorePage::Handle_OnExitButtonClicked()
{
	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveAll(this);
	}
	Close();
}

void UCorePage::Open()
{
	IPageableWidgetInterface::Open();
	DisablePlayerControllerInput = DisablePlayerInput;
	Show();
}

void UCorePage::Close()
{
	IPageableWidgetInterface::Close();
	Hide();
}
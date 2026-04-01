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

#include "UI/CorePage.h"

#include "Components/Button.h"
#include "SubSystems/LocalPlayer/UISubsystem.h"

void UCorePage::NativeConstruct()
{
	Super::NativeConstruct();

	const UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

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
	DisablePlayerInput = false;
	Super::NativeDestruct();
}

void UCorePage::InternalShown()
{
	if (ExitButton)
	{
		ExitButton->OnClicked.AddUniqueDynamic(this, &UCorePage::Handle_OnExitButtonClicked);
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
	DisablePlayerInput = false;
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
	DisablePlayerInput = true;
	Show();
}

void UCorePage::Close()
{
	IPageableWidgetInterface::Close();
	Hide();
}
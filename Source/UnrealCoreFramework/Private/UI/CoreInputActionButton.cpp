// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#include "UI/CoreInputActionButton.h"

#include "CommonTextBlock.h"

void UCoreInputActionButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (UCommonTextBlock* TextBlock = InputActionLabel.Get())
	{
		const FText Text = ButtonLabelText.IsEmpty() ? FText::FromString("Button Label") : ButtonLabelText;
		TextBlock->SetText(Text);
	}
}

void UCoreInputActionButton::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoreInputActionButton::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCoreInputActionButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (UCommonTextBlock* TextBlock = InputActionLabel.Get())
	{
		// TextBlock->SetStyle(GetCurrentTextStyleClass());
	}
}
// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#include "UI/MainUiContainer.h"
#include "UI/CoreStackWidgetContainer.h"

void UMainUiContainer::RemoveWidgetFromStack(UCoreWidget& Widget, const EWidgetContainerType& StackContainerType)
{
	switch (StackContainerType)
	{
		case EWidgetContainerType::HUD:
			if (HUDStack)
			{
				HUDStack->RemoveWidget(Widget);
			}
			break;
		case EWidgetContainerType::Page:
			if (PageStack)
			{
				PageStack->RemoveWidget(Widget);
			}
			break;
		case EWidgetContainerType::Modal:
			if (ModalStack)
			{
				ModalStack->RemoveWidget(Widget);
			}
			break;
	}
}
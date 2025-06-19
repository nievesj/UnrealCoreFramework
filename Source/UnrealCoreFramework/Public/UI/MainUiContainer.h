// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#pragma once

#include "CorePage.h"
#include "CoreStackWidgetContainer.h"

#include "MainUiContainer.generated.h"

UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UMainUiContainer : public UCorePage
{
	GENERATED_BODY()

public:
	template <typename T = UCoreWidget>
	T* AddWidgetToStack(TSubclassOf<UCoreWidget> WidgetClass, const EWidgetContainerType& StackContainerType)
	{
		T* CreatedWidget = nullptr;
		if (WidgetClass && WidgetClass->IsChildOf<T>())
		{
			switch (StackContainerType)
			{
				case EWidgetContainerType::HUD:
					CreatedWidget = HUDStack->AddWidgetToStack<T>(WidgetClass);
					break;
				case EWidgetContainerType::Page:
					CreatedWidget = PageStack->AddWidgetToStack<T>(WidgetClass);
					break;
				case EWidgetContainerType::Modal:
					CreatedWidget = ModalStack->AddWidgetToStack<T>(WidgetClass);
					break;
			}
		}

		return CreatedWidget;
	}

	void RemoveWidgetFromStack(UCoreWidget& Widget, const EWidgetContainerType& StackContainerType);

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* HUDStack;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* PageStack;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* ModalStack;
};

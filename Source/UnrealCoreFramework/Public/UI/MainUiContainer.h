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

	void RemoveWidgetFromStack(UCoreWidget* Widget, const EWidgetContainerType& StackContainerType);

private:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* HUDStack;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* PageStack;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UCoreStackWidgetContainer* ModalStack;
};

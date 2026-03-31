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

#include "UI/CoreStackWidgetContainer.h"
#include "UI/AnimatableWidgetInterface.h"
#include "UI/CoreWidget.h"

DEFINE_LOG_CATEGORY(LogCoreStackWidget);

UCoreStackWidgetContainer::UCoreStackWidgetContainer()
	: bAnimationsEnabled(true)
{
	// Set default animation settings
	DefaultAnimationSettings.WidgetAnimationType = EWidgetAnimationType::CommonUiDefault;
	DefaultAnimationSettings.CommonUiTransitionOptions.TransitionDuration = 0.3f;
	DefaultAnimationSettings.CommonUiTransitionOptions.TransitionType = ECommonSwitcherTransition::FadeOnly;
	DefaultAnimationSettings.CommonUiTransitionOptions.TransitionCurveType = ETransitionCurve::Linear;
}

UCoreWidget* UCoreStackWidgetContainer::AddWidgetWithAnimation(TSubclassOf<UCoreWidget> WidgetClass, FName AnimationPreset)
{
	if (!IsValid(WidgetClass))
	{
		UE_LOG(LogCoreStackWidget, Warning, TEXT("AddWidgetWithAnimation: Invalid widget class"));
		return nullptr;
	}

	UCoreWidget* CreatedWidget = AddWidgetToStack<UCoreWidget>(WidgetClass);
	if (!CreatedWidget)
	{
		return nullptr;
	}

	return CreatedWidget;
}

void UCoreStackWidgetContainer::RemoveWidgetFromStack(UCoreWidget* WidgetToRemove, bool bAnimate, FName AnimationPreset)
{
	if (!WidgetToRemove)
	{
		UE_LOG(LogCoreStackWidget, Warning, TEXT("RemoveWidgetFromStack: Widget is null"));
		return;
	}

	// Remove from active widgets list
	for (int32 i = ActiveWidgets.Num() - 1; i >= 0; --i)
	{
		if (ActiveWidgets[i].Get() == WidgetToRemove)
		{
			ActiveWidgets.RemoveAt(i);
			break;
		}
	}

	// Add to pending removal and play exit animation
	PendingRemovalWidgets.Add(WidgetToRemove);
}

UCoreWidget* UCoreStackWidgetContainer::GetTopWidget() const
{
	if (ActiveWidgets.Num() > 0)
	{
		return ActiveWidgets.Last().Get();
	}
	return nullptr;
}

void UCoreStackWidgetContainer::ClearWidgetStack(bool bAnimate)
{
	// If not animating, clear immediately
	if (!bAnimate || !bAnimationsEnabled)
	{
		for (const TWeakObjectPtr<UCoreWidget>& Widget : ActiveWidgets)
		{
			if (UCoreWidget* CoreWidget = Widget.Get())
			{
				RemoveWidget(*CoreWidget);
			}
		}
		ActiveWidgets.Empty();
		return;
	}

	// Otherwise, animate each widget's removal
	TArray<TWeakObjectPtr<UCoreWidget>> WidgetsToRemove = ActiveWidgets;
	for (const TWeakObjectPtr<UCoreWidget>& Widget : WidgetsToRemove)
	{
		if (UCoreWidget* CoreWidget = Widget.Get())
		{
			RemoveWidgetFromStack(CoreWidget, true);
		}
	}
}

void UCoreStackWidgetContainer::OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget)
{
	if (UCoreWidget* CoreWidget = Cast<UCoreWidget>(&AddedWidget))
	{
		// If widget implements IAnimatableWidgetInterface, get its animation settings
		if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(CoreWidget))
		{
			SetWidgetAnimationSettings(AnimatableWidget->GetWidgetAnimationSettings());
		}
		else
		{
			// Otherwise use default settings
			SetWidgetAnimationSettings(DefaultAnimationSettings);
		}
	}
	Super::OnWidgetAddedToList(AddedWidget);
}

void UCoreStackWidgetContainer::SetWidgetAnimationSettings(const FCoreWidgetAnimationSettings& WidgetAnimationSettings)
{
	// Update common UI transition settings
	if (WidgetAnimationSettings.WidgetAnimationType == EWidgetAnimationType::CommonUiDefault)
	{
		TransitionDuration = WidgetAnimationSettings.CommonUiTransitionOptions.TransitionDuration;
		TransitionType = WidgetAnimationSettings.CommonUiTransitionOptions.TransitionType;
		TransitionCurveType = WidgetAnimationSettings.CommonUiTransitionOptions.TransitionCurveType;
	}
}

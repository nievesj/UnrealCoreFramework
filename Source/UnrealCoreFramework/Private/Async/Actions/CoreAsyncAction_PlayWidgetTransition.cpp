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

#include "Async/Actions/CoreAsyncAction_PlayWidgetTransition.h"

#include "UI/CoreWidget.h"

UCoreAsyncAction_PlayWidgetTransition* UCoreAsyncAction_PlayWidgetTransition::AsyncShowWidget(UCoreWidget* Widget)
{
	UCoreAsyncAction_PlayWidgetTransition* Action = NewObject<UCoreAsyncAction_PlayWidgetTransition>();
	Action->TargetWidget = Widget;
	Action->bIsShow = true;
	if (Widget)
	{
		Action->RegisterWithGameInstance(Widget);
	}
	return Action;
}

UCoreAsyncAction_PlayWidgetTransition* UCoreAsyncAction_PlayWidgetTransition::AsyncHideWidget(UCoreWidget* Widget)
{
	UCoreAsyncAction_PlayWidgetTransition* Action = NewObject<UCoreAsyncAction_PlayWidgetTransition>();
	Action->TargetWidget = Widget;
	Action->bIsShow = false;
	if (Widget)
	{
		Action->RegisterWithGameInstance(Widget);
	}
	return Action;
}

void UCoreAsyncAction_PlayWidgetTransition::Activate()
{
	if (!TargetWidget)
	{
		OnComplete.Broadcast();
		SetReadyToDestroy();
		return;
	}

	TransitionTask = RunTransition();
}

AsyncFlow::TTask<void> UCoreAsyncAction_PlayWidgetTransition::RunTransition()
{
	if (bIsShow)
	{
		co_await TargetWidget->ShowTask();
	}
	else
	{
		co_await TargetWidget->HideTask();
	}

	OnComplete.Broadcast();
	SetReadyToDestroy();
}


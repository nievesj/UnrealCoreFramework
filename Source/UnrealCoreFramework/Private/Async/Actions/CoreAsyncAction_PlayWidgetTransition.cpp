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


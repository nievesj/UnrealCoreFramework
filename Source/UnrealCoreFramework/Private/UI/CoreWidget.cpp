// CoreWidget.cpp
#include "UI/CoreWidget.h"

#include "Async/CoreAsyncTypes.h"
#include "Animation/UMGSequencePlayer.h"
#include "Tools/SubsystemHelper.h"
#include "Tween/TweenManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogCoreWidget);

UUMGSequencePlayer* UCoreWidget::GetSequencePlayer(UWidgetAnimation* InAnimation)
{
	return nullptr;
}

void UCoreWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	TweenManagerSubsystem = USubsystemHelper::GetSubsystem<UTweenManagerSubsystem>(this);
}

void UCoreWidget::Show()
{
	switch (CoreWidgetAnimationSettings.WidgetAnimationType)
	{
		case EWidgetAnimationType::WidgetTween:
			PlayTweenTransition(CoreWidgetAnimationSettings.TweenEntranceOptions, EWidgetTransitionMode::Intro);
			break;
		case EWidgetAnimationType::WidgetAnimation:
			InternalShown();
			break;
		case EWidgetAnimationType::None:
			InternalShown();
			break;
		case EWidgetAnimationType::CommonUiDefault:
			InternalShown();
			break;
	}
}

void UCoreWidget::Hide()
{
	switch (CoreWidgetAnimationSettings.WidgetAnimationType)
	{
		case EWidgetAnimationType::WidgetTween:
			PlayTweenTransition(CoreWidgetAnimationSettings.TweenExitOptions, EWidgetTransitionMode::Outtro);
			break;
		case EWidgetAnimationType::WidgetAnimation:
			InternalHidden();
			break;
		case EWidgetAnimationType::None:
			InternalHidden();
			break;
		case EWidgetAnimationType::CommonUiDefault:
			InternalHidden();
			break;
	}
}

AsyncFlow::TTask<void> UCoreWidget::ShowTask()
{
	if (!IsValid(TweenManagerSubsystem))
	{
		UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] ShowTask — TweenManagerSubsystem is invalid on %s"), *GetName());
		co_return;
	}

	co_await TweenManagerSubsystem->PlayWidgetTransitionEffectTask(
		this, CoreWidgetAnimationSettings.TweenEntranceOptions, EWidgetTransitionMode::Intro);
}

AsyncFlow::TTask<void> UCoreWidget::HideTask()
{
	if (!IsValid(TweenManagerSubsystem))
	{
		UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] HideTask — TweenManagerSubsystem is invalid on %s"), *GetName());
		co_return;
	}

	co_await TweenManagerSubsystem->PlayWidgetTransitionEffectTask(
		this, CoreWidgetAnimationSettings.TweenExitOptions, EWidgetTransitionMode::Outtro);
}

void UCoreWidget::PlayWidgetAnimation(UWidgetAnimation* Anim, const FWidgetAnimationOptions& WidgetAnimationOptions, const EWidgetTransitionMode WidgetTransitionMode)
{
}

void UCoreWidget::InternalShown()
{
	OnShown();
}

void UCoreWidget::InternalHidden()
{
	OnHidden();
}

void UCoreWidget::PlayTweenTransition(const FWidgetTweenTransitionOptions& TweenTransitionOptions, const EWidgetTransitionMode WidgetTransitionMode)
{
	if (!IsValid(TweenManagerSubsystem))
	{
		UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] PlayTweenTransition: TweenManagerSubsystem is null on [%s] — skipping."), *GetName());
		if (WidgetTransitionMode == EWidgetTransitionMode::Intro)
		{
			InternalShown();
		}
		else
		{
			InternalHidden();
		}
		return;
	}

	TweenManagerSubsystem->PlayWidgetTransitionEffect(this, TweenTransitionOptions, WidgetTransitionMode);
}

void UCoreWidget::OnAnimationStarted(const EWidgetTransitionMode& TransitionMode)
{
}

void UCoreWidget::OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode)
{
	switch (TransitionMode)
	{
		case EWidgetTransitionMode::Intro:
			InternalShown();
			break;
		case EWidgetTransitionMode::Outtro:
			InternalHidden();
			break;
	}
}

void UCoreWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	Show();
}

void UCoreWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}
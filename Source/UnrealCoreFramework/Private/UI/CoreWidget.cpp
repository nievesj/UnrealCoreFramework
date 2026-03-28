// CoreWidget.cpp
#include "UI/CoreWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Animation/WidgetAnimation.h"
#include "Async/CoreAsyncTypes.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"
#include "Tools/SubsystemHelper.h"
#include "Tween/TweenManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogCoreWidget);

UUMGSequencePlayer* UCoreWidget::GetSequencePlayer(UWidgetAnimation* InAnimation)
{
	return GetSequencePlayerByAnimation(InAnimation, true);
}

void UCoreWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	TweenManagerSubsystem = USubsystemHelper::GetSubsystem<UTweenManagerSubsystem>(this);
}

void UCoreWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	Show();
}

void UCoreWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	if (bDestroyOnDeactivated)
	{
		RemoveFromParent();
	}
}

void UCoreWidget::Show()
{
	switch (CoreWidgetAnimationSettings.WidgetAnimationType)
	{
		case EWidgetAnimationType::WidgetTween:
			PlayTweenTransition(CoreWidgetAnimationSettings.TweenEntranceOptions, EWidgetTransitionMode::Intro);
			break;
		case EWidgetAnimationType::WidgetAnimation:
			if (IntroAnimation)
			{
				PlayWidgetAnimation(IntroAnimation, IntroAnimationOptions, EWidgetTransitionMode::Intro);
			}
			else
			{
				InternalShown();
			}
			break;
		case EWidgetAnimationType::CommonUiDefault:
		case EWidgetAnimationType::None:
		default:
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
			if (OutroAnimation)
			{
				PlayWidgetAnimation(OutroAnimation, OutroAnimationOptions, EWidgetTransitionMode::Outtro);
			}
			else
			{
				InternalHidden();
			}
			break;
		case EWidgetAnimationType::CommonUiDefault:
		case EWidgetAnimationType::None:
		default:
			InternalHidden();
			break;
	}
}

AsyncFlow::TTask<void> UCoreWidget::ShowTask()
{
	UCF_ASYNC_CONTRACT(this);

	const EWidgetAnimationType AnimType = CoreWidgetAnimationSettings.WidgetAnimationType;

	if (AnimType == EWidgetAnimationType::WidgetAnimation && IntroAnimation)
	{
		PlayAnimation(
			IntroAnimation,
			IntroAnimationOptions.StartAtTime,
			IntroAnimationOptions.NumberOfLoops,
			IntroAnimationOptions.PlayMode,
			IntroAnimationOptions.PlaybackSpeed,
			IntroAnimationOptions.bRestoreState);

		const float Duration = IntroAnimation->GetEndTime() / FMath::Max(IntroAnimationOptions.PlaybackSpeed, 0.01f);
		co_await AsyncFlow::UnpausedDelay(this, Duration);
		InternalShown();
		co_return;
	}

	if (AnimType == EWidgetAnimationType::WidgetTween)
	{
		if (!IsValid(TweenManagerSubsystem))
		{
			UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] ShowTask - TweenManagerSubsystem is invalid on %s"), *GetName());
			InternalShown();
			co_return;
		}

		co_await TweenManagerSubsystem->PlayWidgetTransitionEffectTask(
			this, CoreWidgetAnimationSettings.TweenEntranceOptions, EWidgetTransitionMode::Intro);
		co_return;
	}

	InternalShown();
}

AsyncFlow::TTask<void> UCoreWidget::HideTask()
{
	UCF_ASYNC_CONTRACT(this);

	const EWidgetAnimationType AnimType = CoreWidgetAnimationSettings.WidgetAnimationType;

	if (AnimType == EWidgetAnimationType::WidgetAnimation && OutroAnimation)
	{
		PlayAnimation(
			OutroAnimation,
			OutroAnimationOptions.StartAtTime,
			OutroAnimationOptions.NumberOfLoops,
			OutroAnimationOptions.PlayMode,
			OutroAnimationOptions.PlaybackSpeed,
			OutroAnimationOptions.bRestoreState);

		const float Duration = OutroAnimation->GetEndTime() / FMath::Max(OutroAnimationOptions.PlaybackSpeed, 0.01f);
		co_await AsyncFlow::UnpausedDelay(this, Duration);
		InternalHidden();
		co_return;
	}

	if (AnimType == EWidgetAnimationType::WidgetTween)
	{
		if (!IsValid(TweenManagerSubsystem))
		{
			UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] HideTask - TweenManagerSubsystem is invalid on %s"), *GetName());
			InternalHidden();
			co_return;
		}

		co_await TweenManagerSubsystem->PlayWidgetTransitionEffectTask(
			this, CoreWidgetAnimationSettings.TweenExitOptions, EWidgetTransitionMode::Outtro);
		co_return;
	}

	InternalHidden();
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
		UE_LOG(LogCoreWidget, Warning, TEXT("[CoreWidget] PlayTweenTransition - TweenManagerSubsystem is invalid on %s"), *GetName());
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

void UCoreWidget::PlayWidgetAnimation(UWidgetAnimation* Anim, const FWidgetAnimationOptions& WidgetAnimationOptions, const EWidgetTransitionMode WidgetTransitionMode)
{
	if (!Anim)
	{
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

	PlayAnimation(
		Anim,
		WidgetAnimationOptions.StartAtTime,
		WidgetAnimationOptions.NumberOfLoops,
		WidgetAnimationOptions.PlayMode,
		WidgetAnimationOptions.PlaybackSpeed,
		WidgetAnimationOptions.bRestoreState);
}

void UCoreWidget::OnAnimationStarted(const EWidgetTransitionMode& TransitionMode)
{
}

void UCoreWidget::OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode)
{
}
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
// CoreWidget.cpp
#include "UI/CoreWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Async/CoreAsyncTypes.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"
#include "Tools/SubsystemHelper.h"
#include "Tween/TweenManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogCoreWidget);


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
			PlayTweenTransition(CoreWidgetAnimationSettings.TweenExitOptions, EWidgetTransitionMode::Outro);
			break;
		case EWidgetAnimationType::WidgetAnimation:
			if (OutroAnimation)
			{
				PlayWidgetAnimation(OutroAnimation, OutroAnimationOptions, EWidgetTransitionMode::Outro);
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
			this, CoreWidgetAnimationSettings.TweenExitOptions, EWidgetTransitionMode::Outro);
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


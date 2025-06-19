#pragma once
#include "UI/CoreWidget.h"

#include "Animation/UMGSequencePlayer.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Tools/SubsystemHelper.h"
#include "Tween/TweenManagerSubsystem.h"
#include "VisualLogger/VisualLogger.h"

DEFINE_LOG_CATEGORY(LogCoreWidget);

UUMGSequencePlayer* UCoreWidget::GetSequencePlayer(UWidgetAnimation* InAnimation)
{
	return nullptr; // GetOrAddSequencePlayer(InAnimation);
}

void UCoreWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	// Get the animation helper
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
			/*if (WidgetAnimationIntro)
			{
				PlayWidgetAnimation(WidgetAnimationIntro, WidgetAnimationOptionsIntro, EWidgetTransitionMode::Intro);
			}*/
			break;
		case EWidgetAnimationType::None:
			break;
		case EWidgetAnimationType::CommonUiDefault:
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
			break;
		case EWidgetAnimationType::None:
			break;
		case EWidgetAnimationType::CommonUiDefault:
			break;
	}
}

void UCoreWidget::PlayWidgetAnimation(UWidgetAnimation* Anim, const FWidgetAnimationOptions& WidgetAnimationOptions, const EWidgetTransitionMode WidgetTransitionMode)
{
	/*UUMGSequencePlayer*         Player = GetOrAddSequencePlayer(Anim);
	TWeakObjectPtr<UCoreWidget> WeakThis = this;
	Player->OnSequenceFinishedPlaying().AddLambda(
		[WidgetTransitionMode, WeakThis](UUMGSequencePlayer& Player) {
			if (UCoreWidget* Widget = WeakThis.Get())
			{
				Widget->OnAnimationCompleted(WidgetTransitionMode);
			}
		}
		);

	PlayAnimation(Anim, WidgetAnimationOptions.StartAtTime, WidgetAnimationOptions.NumberOfLoops, WidgetAnimationOptions.PlayMode, WidgetAnimationOptions.PlaybackSpeed);*/
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
		return;

	TweenManagerSubsystem->PlayWidgetTransitionEffect(this, TweenTransitionOptions, WidgetTransitionMode);
}

void UCoreWidget::OnAnimationStarted(const EWidgetTransitionMode& TransitionMode)
{
	UE_LOG(LogTemp, Error, TEXT("OnAnimationStarted%s"), *GetName());
}

void UCoreWidget::OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode)
{
	UE_LOG(LogTemp, Error, TEXT("OnAnimationCompleted %s"), *GetName());
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
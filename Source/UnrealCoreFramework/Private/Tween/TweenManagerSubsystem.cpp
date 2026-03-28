#include "Tween/TweenManagerSubsystem.h"

#include "Async/CoreAsyncTypes.h"
#include "CoreTween.h"
#include "CoreTweenBuilder.h"
#include "CoreTweenEasing.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "UI/AnimatableWidgetInterface.h"
#include "UI/CoreWidget.h"
#include "UI/UiCoreFrameworkTypes.h"
#include "UnrealClient.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"

void UTweenManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTweenManagerSubsystem::Deinitialize()
{
	if (ActiveAnimationTask.IsValid() && !ActiveAnimationTask.IsCompleted())
	{
		ActiveAnimationTask.Cancel();
	}
	Super::Deinitialize();
}

void UTweenManagerSubsystem::PlayWidgetTransitionEffect(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	if (!Widget)
	{
		return;
	}

	if (!ShouldPlayAnimations())
	{
		if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
		{
			AnimatableWidget->OnAnimationCompleted(TransitionMode);
		}
		return;
	}

	AsyncFlow::TTask<void> Task;
	switch (TransitionOptions.TransitionType)
	{
		case EWidgetTransitionType::Scale:
			Task = PlayScaleAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		case EWidgetTransitionType::Translation:
			Task = PlayTranslationAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		case EWidgetTransitionType::Fade:
			Task = PlayFadeAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		default:
			if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
			{
				AnimatableWidget->OnAnimationCompleted(TransitionMode);
			}
			return;
	}

	if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
	{
		AnimatableWidget->OnAnimationStarted(TransitionMode);
	}

	ActiveAnimationTask = MoveTemp(Task);
	ActiveAnimationTask.SetDebugName(FString::Printf(TEXT("WidgetTransition_%s"), *Widget->GetName()));
	ActiveAnimationTask.Start();
}

AsyncFlow::TTask<void> UTweenManagerSubsystem::PlayWidgetTransitionEffectTask(
	UCoreWidget* Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode TransitionMode)
{
	UCF_ASYNC_CONTRACT(this);

	if (!Widget)
	{
		co_return;
	}

	if (!ShouldPlayAnimations())
	{
		if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
		{
			AnimatableWidget->OnAnimationCompleted(TransitionMode);
		}
		co_return;
	}

	switch (TransitionOptions.TransitionType)
	{
		case EWidgetTransitionType::Scale:
		{
			if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
			{
				AnimatableWidget->OnAnimationStarted(TransitionMode);
			}
			co_await PlayScaleAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		}
		case EWidgetTransitionType::Translation:
		{
			if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
			{
				AnimatableWidget->OnAnimationStarted(TransitionMode);
			}
			co_await PlayTranslationAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		}
		case EWidgetTransitionType::Fade:
		{
			if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
			{
				AnimatableWidget->OnAnimationStarted(TransitionMode);
			}
			co_await PlayFadeAnimationTask(Widget, TransitionOptions, TransitionMode);
			break;
		}
		default:
		{
			if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
			{
				AnimatableWidget->OnAnimationCompleted(TransitionMode);
			}
			break;
		}
	}

	co_return;
}

bool UTweenManagerSubsystem::PlayPresetAnimation(
	UCoreWidget*		  Widget,
	const FName&		  PresetName,
	EWidgetTransitionMode TransitionMode)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTweenManagerSubsystem: Cannot play preset for null widget"));
		return false;
	}

	if (!AnimationPresets.Contains(PresetName))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTweenManagerSubsystem: Animation preset '%s' not found"), *PresetName.ToString());
		return false;
	}

	PlayWidgetTransitionEffect(Widget, AnimationPresets[PresetName], TransitionMode);
	return true;
}

void UTweenManagerSubsystem::RegisterAnimationPresets(const TMap<FName, FWidgetTweenTransitionOptions>& Presets)
{
	AnimationPresets.Append(Presets);
}

bool UTweenManagerSubsystem::ShouldPlayAnimations() const
{
	return true;
}

AsyncFlow::TTask<void> UTweenManagerSubsystem::PlayScaleAnimationTask(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	UCF_ASYNC_CONTRACT(this);

	const FVector2D Start = TransitionOptions.ScaleFrom;
	const FVector2D End = TransitionOptions.ScaleTo;
	const float		StartOpacity = TransitionOptions.FadeFrom;
	const float		EndOpacity = TransitionOptions.FadeTo;

	co_await CreateAndPlayTweenTask(Widget,
		Start, End,
		FVector2D::ZeroVector, FVector2D::ZeroVector,
		StartOpacity, EndOpacity,
		TransitionOptions.TransitionTime,
		TransitionOptions.EasingType,
		TransitionMode);
}

AsyncFlow::TTask<void> UTweenManagerSubsystem::PlayTranslationAnimationTask(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	UCF_ASYNC_CONTRACT(this);

	Widget->ForceLayoutPrepass();

	co_await AsyncFlow::NextTick(this);

	TWeakObjectPtr<UCoreWidget> WeakWidget = Widget;
	if (!WeakWidget.IsValid())
	{
		co_return;
	}

	FVector2D Start = FVector2D::ZeroVector;
	FVector2D End = FVector2D::ZeroVector;

	if (TransitionOptions.UseViewportAsTranslationOrigin)
	{
		GetViewportTranslationVectors(TransitionOptions.WidgetTranslationType, Start, End);
		Start += TransitionOptions.TranslationFromOffset;
		End += TransitionOptions.TranslationToOffset;

		if (TransitionMode == EWidgetTransitionMode::Outtro)
		{
			Swap(Start, End);
		}
	}
	else
	{
		Start = TransitionOptions.TranslationFrom + TransitionOptions.TranslationFromOffset;
		End = TransitionOptions.TranslationTo + TransitionOptions.TranslationToOffset;
	}

	co_await CreateAndPlayTweenTask(WeakWidget.Get(),
		FVector2D::UnitVector, FVector2D::UnitVector,
		Start, End,
		TransitionOptions.FadeFrom, TransitionOptions.FadeTo,
		TransitionOptions.TransitionTime,
		TransitionOptions.EasingType,
		TransitionMode);
}

AsyncFlow::TTask<void> UTweenManagerSubsystem::PlayFadeAnimationTask(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	UCF_ASYNC_CONTRACT(this);

	co_await CreateAndPlayTweenTask(Widget,
		FVector2D::UnitVector, FVector2D::UnitVector,
		FVector2D::ZeroVector, FVector2D::ZeroVector,
		TransitionOptions.FadeFrom, TransitionOptions.FadeTo,
		TransitionOptions.TransitionTime,
		TransitionOptions.EasingType,
		TransitionMode);
}

void UTweenManagerSubsystem::GetViewportTranslationVectors(
	const EWidgetTranslationType TranslationType,
	FVector2D&					 OutStart,
	FVector2D&					 OutEnd)
{
	if (!GEngine || !GEngine->GameViewport || !GEngine->GameViewport->Viewport)
	{
		OutStart = FVector2D::ZeroVector;
		OutEnd = FVector2D::ZeroVector;
		return;
	}

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	switch (TranslationType)
	{
		case EWidgetTranslationType::FromLeft:
			OutStart = FVector2D(-ViewportSize.X, 0);
			break;
		case EWidgetTranslationType::FromRight:
			OutStart = FVector2D(ViewportSize.X, 0);
			break;
		case EWidgetTranslationType::FromTop:
			OutStart = FVector2D(0, -ViewportSize.Y);
			break;
		case EWidgetTranslationType::FromBottom:
			OutStart = FVector2D(0, ViewportSize.Y);
			break;
		default:
			OutStart = FVector2D::ZeroVector;
			break;
	}
}

AsyncFlow::TTask<void> UTweenManagerSubsystem::CreateAndPlayTweenTask(
	UCoreWidget*	 Widget,
	const FVector2D& StartScale, const FVector2D& EndScale,
	const FVector2D& StartTranslation, const FVector2D& EndTranslation,
	const float StartOpacity, const float EndOpacity, const float Duration,
	const ECoreTweenEasingType EasingType, const EWidgetTransitionMode TransitionMode)
{
	UCF_ASYNC_CONTRACT(this);

	if (!Widget)
	{
		co_return;
	}

	co_await UCoreTween::Create(Widget, Duration)
		.FromScale(StartScale)
		.ToScale(EndScale)
		.FromTranslation(StartTranslation)
		.ToTranslation(EndTranslation)
		.FromOpacity(StartOpacity)
		.ToOpacity(EndOpacity)
		.Easing(EasingType)
		.Run(this);

	if (IsValid(Widget))
	{
		if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
		{
			AnimatableWidget->OnAnimationCompleted(TransitionMode);
		}
	}
}

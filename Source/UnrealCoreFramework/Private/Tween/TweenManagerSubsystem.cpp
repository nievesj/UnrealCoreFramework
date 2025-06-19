#include "Tween/TweenManagerSubsystem.h"

#include "BUITween.h"
#include "BUITweenInstance.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "TimerManager.h"
#include "Tools/WaitUntilNextFrameTaskRunner.h"
#include "UI/AnimatableWidgetInterface.h"
#include "UI/CoreWidget.h"
#include "UI/UiCoreFrameworkTypes.h"
#include "UnrealClient.h"

void UTweenManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTweenManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UTweenManagerSubsystem::IsTickable() const
{
	return true;
}

TStatId UTweenManagerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTweenManagerSubsystem, STATGROUP_Tickables);
}

void UTweenManagerSubsystem::Tick(float DeltaTime)
{
}

void UTweenManagerSubsystem::PlayWidgetTransitionEffect(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidgetAnimationSubsystem: Cannot play transition for null widget"));
		return;
	}

	if (!ShouldPlayAnimations())
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Playing transition effect for widget %s, type: %d, mode: %d"),
		*Widget->GetName(), (int32)TransitionOptions.TransitionType, (int32)TransitionMode);

	if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(Widget))
	{
		AnimatableWidget->OnAnimationStarted(TransitionMode);
	}

	switch (TransitionOptions.TransitionType)
	{
		case EWidgetTransitionType::Scale:
			PlayScaleAnimation(Widget, TransitionOptions, TransitionMode);
			break;
		case EWidgetTransitionType::Translation:
			PlayTranslationAnimation(Widget, TransitionOptions, TransitionMode);
			break;
		case EWidgetTransitionType::Fade:
			PlayFadeAnimation(Widget, TransitionOptions, TransitionMode);
			break;
		default:
			break;
	}
}

bool UTweenManagerSubsystem::PlayPresetAnimation(
	UCoreWidget*		  Widget,
	const FName&		  PresetName,
	EWidgetTransitionMode TransitionMode)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidgetAnimationSubsystem: Cannot play preset for null widget"));
		return false;
	}

	if (!AnimationPresets.Contains(PresetName))
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidgetAnimationSubsystem: Animation preset '%s' not found"), *PresetName.ToString());
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

void UTweenManagerSubsystem::PlayScaleAnimation(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	const FVector2D Start = TransitionOptions.ScaleFrom;
	const FVector2D End = TransitionOptions.ScaleTo;
	const float		StartOpacity = TransitionOptions.FadeFrom;
	const float		EndOpacity = TransitionOptions.FadeTo;

	CreateAndPlayTween(Widget,
		Start, End,
		FVector2D::ZeroVector, FVector2D::ZeroVector,
		StartOpacity, EndOpacity,
		TransitionOptions.TransitionTime,
		TransitionOptions.EasingType,
		TransitionMode);
}

void UTweenManagerSubsystem::PlayTranslationAnimation(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	// Ensure the widget's layout is up to date before calculating geometry.
	Widget->ForceLayoutPrepass();

	// Use weak pointers to safely reference the widget and subsystem inside the lambda.
	TWeakObjectPtr<UCoreWidget>			   WeakWidget = Widget;
	TWeakObjectPtr<UTweenManagerSubsystem> WeakThis = this;

	// Run the animation logic on the next frame to ensure geometry is valid.
	UWaitUntilNextFrameTaskRunner* Runner = NewObject<UWaitUntilNextFrameTaskRunner>(this);

	TFuture<void> Future = Runner->Run([WeakWidget, WeakThis, TransitionOptions, TransitionMode]() {
		if (!WeakWidget.IsValid() || !WeakThis.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Widget or subsystem no longer valid during translation animation"));
			return;
		}

		FVector2D Start = FVector2D::ZeroVector;
		FVector2D End = FVector2D::ZeroVector;

		// Determine translation vectors based on whether viewport origin is used.
		if (TransitionOptions.UseViewportAsTranslationOrigin)
		{
			// Get updated viewport values and apply offsets.
			GetViewportTranslationVectors(TransitionOptions.WidgetTranslationType, Start, End);
			Start += TransitionOptions.TranslationFromOffset;
			End += TransitionOptions.TranslationToOffset;

			// Swap start and end for outro animations.
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

		// Start the tween animation with the calculated parameters.
		WeakThis->CreateAndPlayTween(WeakWidget.Get(),
			FVector2D::UnitVector, FVector2D::UnitVector,
			Start, End,
			TransitionOptions.FadeFrom, TransitionOptions.FadeTo,
			TransitionOptions.TransitionTime,
			TransitionOptions.EasingType,
			TransitionMode);
	});
}

void UTweenManagerSubsystem::PlayFadeAnimation(
	UCoreWidget*						 Widget,
	const FWidgetTweenTransitionOptions& TransitionOptions,
	EWidgetTransitionMode				 TransitionMode)
{
	UE_LOG(LogTemp, Log, TEXT("UWidgetAnimationSubsystem: Playing fade animation for widget %s with FadeFrom=%f, FadeTo=%f, Duration=%f"),
		*Widget->GetName(), TransitionOptions.FadeFrom, TransitionOptions.FadeTo, TransitionOptions.TransitionTime);

	CreateAndPlayTween(Widget,
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

void UTweenManagerSubsystem::CreateAndPlayTween(
	UCoreWidget*	 Widget,
	const FVector2D& StartScale, const FVector2D& EndScale,
	const FVector2D& StartTranslation, const FVector2D& EndTranslation,
	float StartOpacity, float EndOpacity, float Duration,
	EBUIEasingType EasingType, EWidgetTransitionMode TransitionMode)
{
	if (!Widget)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidgetAnimationSubsystem: Invalid widget for tween animation"));
		return;
	}

	FBUITweenInstance& Tween = UBUITween::Create(Widget, Duration)
								   .FromScale(StartScale)
								   .ToScale(EndScale)
								   .FromTranslation(StartTranslation)
								   .ToTranslation(EndTranslation)
								   .FromOpacity(StartOpacity)
								   .ToOpacity(EndOpacity)
								   .Easing(EasingType)
								   .OnComplete(FBUITweenSignature::CreateLambda(
									   [TransitionMode](UWidget* InWidget) {
										   if (!IsValid(InWidget))
										   {
											   UE_LOG(LogTemp, Error, TEXT("UWidgetAnimationSubsystem: Widget no longer valid in tween completion"));
											   return;
										   }
										   if (IAnimatableWidgetInterface* AnimatableWidget = Cast<IAnimatableWidgetInterface>(InWidget))
										   {
											   AnimatableWidget->OnAnimationCompleted(TransitionMode);
										   }
									   }));

	Tween.Begin();
}
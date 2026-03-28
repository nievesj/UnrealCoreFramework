#include "CoreTweenBuilder.h"

#include "CoreTween.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"
#include "Engine/World.h"
#include "Misc/App.h"

FCoreTweenBuilder::FCoreTweenBuilder(
	TSharedPtr<ICoreTweenTarget> InTarget,
	UWidget* InWidget,
	const float InDuration,
	const float InDelay,
	const bool bInAdditive)
	: Target(MoveTemp(InTarget))
	, WidgetPtr(InWidget)
	, Duration(InDuration)
	, Delay(InDelay)
	, bAdditive(bInAdditive)
{
}

// ── Property setters ────────────────────────────────────────────────

FCoreTweenBuilder& FCoreTweenBuilder::FromTranslation(const FVector2D& Value) { TranslationProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToTranslation(const FVector2D& Value) { TranslationProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromScale(const FVector2D& Value) { ScaleProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToScale(const FVector2D& Value) { ScaleProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromOpacity(const float Value) { OpacityProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToOpacity(const float Value) { OpacityProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromColor(const FLinearColor& Value) { ColorProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToColor(const FLinearColor& Value) { ColorProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromRotation(const float Value) { RotationProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToRotation(const float Value) { RotationProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromCanvasPosition(const FVector2D& Value) { CanvasPositionProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToCanvasPosition(const FVector2D& Value) { CanvasPositionProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromPadding(const FMargin& Value) { PaddingProp.SetStart(FVector4(Value.Left, Value.Top, Value.Right, Value.Bottom)); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToPadding(const FMargin& Value) { PaddingProp.SetTarget(FVector4(Value.Left, Value.Top, Value.Right, Value.Bottom)); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromVisibility(const ESlateVisibility Value) { VisibilityProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToVisibility(const ESlateVisibility Value) { VisibilityProp.SetTarget(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::FromMaxDesiredHeight(const float Value) { MaxDesiredHeightProp.SetStart(Value); return *this; }
FCoreTweenBuilder& FCoreTweenBuilder::ToMaxDesiredHeight(const float Value) { MaxDesiredHeightProp.SetTarget(Value); return *this; }

// ── Configuration ───────────────────────────────────────────────────

FCoreTweenBuilder& FCoreTweenBuilder::Easing(const ECoreTweenEasingType InType, const TOptional<float> InParam)
{
	EasingType = InType;
	EasingParam = InParam;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::ToReset()
{
	ScaleProp.SetTarget(FVector2D::UnitVector);
	OpacityProp.SetTarget(1.0f);
	TranslationProp.SetTarget(FVector2D::ZeroVector);
	ColorProp.SetTarget(FLinearColor::White);
	RotationProp.SetTarget(0.0f);
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::SetDelay(const float Seconds)
{
	Delay = Seconds;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::SetLoops(const int32 Count)
{
	LoopCount = Count;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::SetPingPong(const bool bInPingPong)
{
	bPingPong = bInPingPong;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::SetTimeSource(const ECoreTweenTimeSource Source)
{
	TimeSource = Source;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::OnStart(const FCoreTweenSignature& InDelegate)
{
	OnStartDelegate = InDelegate;
	return *this;
}

FCoreTweenBuilder& FCoreTweenBuilder::OnComplete(const FCoreTweenSignature& InDelegate)
{
	OnCompleteDelegate = InDelegate;
	return *this;
}

// ── Internal helpers ────────────────────────────────────────────────

void FCoreTweenBuilder::BeginAllProperties()
{
	if (!Target || !Target->IsTargetValid())
	{
		return;
	}

	TranslationProp.OnBegin(Target->GetCurrentTranslation());
	ScaleProp.OnBegin(Target->GetCurrentScale());
	OpacityProp.OnBegin(Target->GetCurrentOpacity());
	ColorProp.OnBegin(Target->GetCurrentColor());
	RotationProp.OnBegin(Target->GetCurrentRotation());
	CanvasPositionProp.OnBegin(Target->GetCurrentCanvasPosition());
	PaddingProp.OnBegin(Target->GetCurrentPadding());
	VisibilityProp.OnBegin(Target->GetCurrentVisibility());
	MaxDesiredHeightProp.OnBegin(Target->GetCurrentMaxDesiredHeight());
}

void FCoreTweenBuilder::ApplyAll(const float EasedAlpha)
{
	if (!Target || !Target->IsTargetValid())
	{
		return;
	}

	if (ColorProp.IsSet())
	{
		ColorProp.Update(EasedAlpha);
		Target->ApplyColor(ColorProp.CurrentValue);
	}

	if (OpacityProp.IsSet())
	{
		OpacityProp.Update(EasedAlpha);
		Target->ApplyOpacity(OpacityProp.CurrentValue);
	}

	if (VisibilityProp.IsSet())
	{
		if (VisibilityProp.Update(EasedAlpha))
		{
			Target->ApplyVisibility(VisibilityProp.CurrentValue);
		}
	}

	bool bChangedRenderTransform = false;

	if (TranslationProp.IsSet())
	{
		TranslationProp.Update(EasedAlpha);
		bChangedRenderTransform = true;
	}
	if (ScaleProp.IsSet())
	{
		ScaleProp.Update(EasedAlpha);
		bChangedRenderTransform = true;
	}
	if (RotationProp.IsSet())
	{
		if (RotationProp.Update(EasedAlpha))
		{
			bChangedRenderTransform = true;
		}
	}

	if (bChangedRenderTransform)
	{
		if (TranslationProp.IsSet())
		{
			Target->ApplyTranslation(TranslationProp.CurrentValue);
		}
		if (ScaleProp.IsSet())
		{
			Target->ApplyScale(ScaleProp.CurrentValue);
		}
		if (RotationProp.IsSet())
		{
			Target->ApplyRotation(RotationProp.CurrentValue);
		}
	}

	if (CanvasPositionProp.IsSet())
	{
		if (CanvasPositionProp.Update(EasedAlpha))
		{
			Target->ApplyCanvasPosition(CanvasPositionProp.CurrentValue);
		}
	}

	if (PaddingProp.IsSet())
	{
		if (PaddingProp.Update(EasedAlpha))
		{
			Target->ApplyPadding(PaddingProp.CurrentValue);
		}
	}

	if (MaxDesiredHeightProp.IsSet())
	{
		if (MaxDesiredHeightProp.Update(EasedAlpha))
		{
			Target->ApplyMaxDesiredHeight(MaxDesiredHeightProp.CurrentValue);
		}
	}
}

// ── Coroutine runner ────────────────────────────────────────────────

AsyncFlow::TTask<void> FCoreTweenBuilder::Run(UObject* WorldContext)
{
	// Move all builder state into coroutine-local variables before any suspension.
	// The builder may be a temporary destroyed after the first co_await.
	TSharedPtr<ICoreTweenTarget> LocalTarget = MoveTemp(Target);
	const float LocalDuration = Duration;
	const float LocalDelay = Delay;
	const ECoreTweenEasingType LocalEasingType = EasingType;
	const TOptional<float> LocalEasingParam = EasingParam;
	const int32 LocalLoopCount = LoopCount;
	const bool bLocalPingPong = bPingPong;
	const ECoreTweenTimeSource LocalTimeSource = TimeSource;

	TCoreTweenProp<FVector2D> LocalTranslationProp = MoveTemp(TranslationProp);
	TCoreTweenProp<FVector2D> LocalScaleProp = MoveTemp(ScaleProp);
	TCoreTweenProp<FLinearColor> LocalColorProp = MoveTemp(ColorProp);
	TCoreTweenProp<float> LocalOpacityProp = MoveTemp(OpacityProp);
	TCoreTweenProp<float> LocalRotationProp = MoveTemp(RotationProp);
	TCoreTweenProp<FVector2D> LocalCanvasPositionProp = MoveTemp(CanvasPositionProp);
	TCoreTweenProp<FVector4> LocalPaddingProp = MoveTemp(PaddingProp);
	TCoreTweenInstantProp<ESlateVisibility> LocalVisibilityProp = MoveTemp(VisibilityProp);
	TCoreTweenProp<float> LocalMaxDesiredHeightProp = MoveTemp(MaxDesiredHeightProp);

	FCoreTweenSignature LocalOnStartDelegate = MoveTemp(OnStartDelegate);
	FCoreTweenSignature LocalOnCompleteDelegate = MoveTemp(OnCompleteDelegate);

	// Lambda that applies all active properties to the target
	auto ApplyAllLocal = [&](float EasedAlpha)
	{
		if (!LocalTarget || !LocalTarget->IsTargetValid())
		{
			return;
		}

		if (LocalColorProp.IsSet())
		{
			LocalColorProp.Update(EasedAlpha);
			LocalTarget->ApplyColor(LocalColorProp.CurrentValue);
		}
		if (LocalOpacityProp.IsSet())
		{
			LocalOpacityProp.Update(EasedAlpha);
			LocalTarget->ApplyOpacity(LocalOpacityProp.CurrentValue);
		}
		if (LocalVisibilityProp.IsSet())
		{
			if (LocalVisibilityProp.Update(EasedAlpha))
			{
				LocalTarget->ApplyVisibility(LocalVisibilityProp.CurrentValue);
			}
		}

		bool bChangedRenderTransform = false;
		if (LocalTranslationProp.IsSet())
		{
			LocalTranslationProp.Update(EasedAlpha);
			bChangedRenderTransform = true;
		}
		if (LocalScaleProp.IsSet())
		{
			LocalScaleProp.Update(EasedAlpha);
			bChangedRenderTransform = true;
		}
		if (LocalRotationProp.IsSet())
		{
			if (LocalRotationProp.Update(EasedAlpha))
			{
				bChangedRenderTransform = true;
			}
		}
		if (bChangedRenderTransform)
		{
			if (LocalTranslationProp.IsSet())
			{
				LocalTarget->ApplyTranslation(LocalTranslationProp.CurrentValue);
			}
			if (LocalScaleProp.IsSet())
			{
				LocalTarget->ApplyScale(LocalScaleProp.CurrentValue);
			}
			if (LocalRotationProp.IsSet())
			{
				LocalTarget->ApplyRotation(LocalRotationProp.CurrentValue);
			}
		}

		if (LocalCanvasPositionProp.IsSet())
		{
			if (LocalCanvasPositionProp.Update(EasedAlpha))
			{
				LocalTarget->ApplyCanvasPosition(LocalCanvasPositionProp.CurrentValue);
			}
		}
		if (LocalPaddingProp.IsSet())
		{
			if (LocalPaddingProp.Update(EasedAlpha))
			{
				LocalTarget->ApplyPadding(LocalPaddingProp.CurrentValue);
			}
		}
		if (LocalMaxDesiredHeightProp.IsSet())
		{
			if (LocalMaxDesiredHeightProp.Update(EasedAlpha))
			{
				LocalTarget->ApplyMaxDesiredHeight(LocalMaxDesiredHeightProp.CurrentValue);
			}
		}
	};

	// Tween state for external tracking
	TSharedPtr<FCoreTweenState> TweenState = MakeShared<FCoreTweenState>();
	TweenState->Widget = WidgetPtr;
	UCoreTween::RegisterTweenState(WorldContext, TweenState);

	// Read current values from target for "from current" defaults
	if (LocalTarget && LocalTarget->IsTargetValid())
	{
		LocalTranslationProp.OnBegin(LocalTarget->GetCurrentTranslation());
		LocalScaleProp.OnBegin(LocalTarget->GetCurrentScale());
		LocalOpacityProp.OnBegin(LocalTarget->GetCurrentOpacity());
		LocalColorProp.OnBegin(LocalTarget->GetCurrentColor());
		LocalRotationProp.OnBegin(LocalTarget->GetCurrentRotation());
		LocalCanvasPositionProp.OnBegin(LocalTarget->GetCurrentCanvasPosition());
		LocalPaddingProp.OnBegin(LocalTarget->GetCurrentPadding());
		LocalVisibilityProp.OnBegin(LocalTarget->GetCurrentVisibility());
		LocalMaxDesiredHeightProp.OnBegin(LocalTarget->GetCurrentMaxDesiredHeight());
	}

	// Apply starting state immediately (eased alpha = 0)
	ApplyAllLocal(0.0f);

	// Delay phase
	if (LocalDelay > 0.0f)
	{
		switch (LocalTimeSource)
		{
			case ECoreTweenTimeSource::GameTime:
				co_await AsyncFlow::Delay(WorldContext, LocalDelay);
				break;
			case ECoreTweenTimeSource::Unpaused:
				co_await AsyncFlow::UnpausedDelay(WorldContext, LocalDelay);
				break;
			case ECoreTweenTimeSource::RealTime:
				co_await AsyncFlow::RealDelay(WorldContext, LocalDelay);
				break;
		}
	}

	// Fire OnStart callback
	LocalOnStartDelegate.ExecuteIfBound();

	// Store flow state for external cancellation
	AsyncFlow::FAsyncFlowState* CurrentFlowState = AsyncFlow::Private::GetCurrentFlowState();
	if (CurrentFlowState)
	{
		TweenState->FlowState = MakeShareable(CurrentFlowState, [](AsyncFlow::FAsyncFlowState*) {});
	}

	// Interpolation loop
	float ElapsedTime = 0.0f;
	int32 LoopsRemaining = LocalLoopCount;
	bool bForward = true;

	while (true)
	{
		co_await AsyncFlow::NextTick(WorldContext);

		if (!LocalTarget || !LocalTarget->IsTargetValid())
		{
			TweenState->FlowState.Reset();
			TweenState->bFinished.store(true, std::memory_order_release);
			co_return;
		}

		if (TweenState->bForceComplete.load(std::memory_order_acquire))
		{
			ApplyAllLocal(FCoreTweenEasing::Ease(LocalEasingType, 1.0f, LocalEasingParam));
			LocalOnCompleteDelegate.ExecuteIfBound();
			TweenState->FlowState.Reset();
			TweenState->bFinished.store(true, std::memory_order_release);
			co_return;
		}

		float DeltaTime = 0.0f;
		switch (LocalTimeSource)
		{
			case ECoreTweenTimeSource::GameTime:
			{
				UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
				DeltaTime = World ? World->GetDeltaSeconds() : FApp::GetDeltaTime();
				break;
			}
			case ECoreTweenTimeSource::Unpaused:
			case ECoreTweenTimeSource::RealTime:
				DeltaTime = FApp::GetDeltaTime();
				break;
		}

		ElapsedTime += (bForward ? DeltaTime : -DeltaTime);
		ElapsedTime = FMath::Clamp(ElapsedTime, 0.0f, LocalDuration);

		const float NormalizedAlpha = LocalDuration > 0.0f ? ElapsedTime / LocalDuration : 1.0f;
		const float EasedAlpha = FCoreTweenEasing::Ease(LocalEasingType, NormalizedAlpha, LocalEasingParam);

		ApplyAllLocal(EasedAlpha);

		const bool bLapComplete = bForward ? (ElapsedTime >= LocalDuration) : (ElapsedTime <= 0.0f);
		if (bLapComplete)
		{
			if (bLocalPingPong)
			{
				bForward = !bForward;
				if (!bForward)
				{
					continue;
				}
			}

			if (LoopsRemaining > 0)
			{
				LoopsRemaining--;
			}
			if (LoopsRemaining == 0)
			{
				break;
			}

			ElapsedTime = 0.0f;
			bForward = true;
		}
	}

	// Fire OnComplete callback
	LocalOnCompleteDelegate.ExecuteIfBound();
	TweenState->FlowState.Reset();
	TweenState->bFinished.store(true, std::memory_order_release);
}


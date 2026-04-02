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

#pragma once

#include "CoreTweenEasing.h"
#include "CoreTweenProperty.h"
#include "CoreTweenTypes.h"
#include "Targets/CoreTweenTarget.h"
#include "AsyncFlowTask.h"
#include "Math/Vector2D.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Layout/Margin.h"
#include "Components/SlateWrapperTypes.h"
#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtrTemplates.h"

#include <atomic>

class UWidget;
class UObject;

/**
 * Shared mutable state for a running tween. Created by Run() and shared
 * between the coroutine frame and the external tracking system (UCoreTween).
 *
 * The TTask returned by Run() is exclusively owned by the caller.
 * Cancellation and force-complete route through this shared state.
 */
struct CORETWEEN_API FCoreTweenState
{
	/** Set by CompleteAll(). The coroutine loop checks this each tick. */
	std::atomic<bool> bForceComplete{ false };

	/** The flow state of the TTask running this tween. Used for cancellation. */
	TSharedPtr<AsyncFlow::FAsyncFlowState> FlowState;

	/** The widget being tweened. Used by GetIsTweening()/Clear(). */
	TWeakObjectPtr<UWidget> Widget;

	/** True once the coroutine has finished (naturally, cancelled, or force-completed). */
	std::atomic<bool> bFinished{ false };

	/** Keeps the coroutine alive until completion. Cleared when finished. */
	AsyncFlow::TTask<void> RunningTask;
};

/**
 * Fluent tween configuration. Returned by value from UCoreTween::Create().
 * Chain setters return *this by reference. Call Run() to consume the builder
 * and start the coroutine — the builder should not be used after Run().
 */
class CORETWEEN_API FCoreTweenBuilder
{
public:
	FCoreTweenBuilder() = default;
	FCoreTweenBuilder(TSharedPtr<ICoreTweenTarget> InTarget, UWidget* InWidget, float InDuration, float InDelay, bool bInAdditive);

	// ── Property setters ────────────────────────────────────────────

	FCoreTweenBuilder& FromTranslation(const FVector2D& Value);
	FCoreTweenBuilder& ToTranslation(const FVector2D& Value);
	FCoreTweenBuilder& FromScale(const FVector2D& Value);
	FCoreTweenBuilder& ToScale(const FVector2D& Value);
	FCoreTweenBuilder& FromOpacity(float Value);
	FCoreTweenBuilder& ToOpacity(float Value);
	FCoreTweenBuilder& FromColor(const FLinearColor& Value);
	FCoreTweenBuilder& ToColor(const FLinearColor& Value);
	FCoreTweenBuilder& FromRotation(float Value);
	FCoreTweenBuilder& ToRotation(float Value);
	FCoreTweenBuilder& FromCanvasPosition(const FVector2D& Value);
	FCoreTweenBuilder& ToCanvasPosition(const FVector2D& Value);
	FCoreTweenBuilder& FromPadding(const FMargin& Value);
	FCoreTweenBuilder& ToPadding(const FMargin& Value);
	FCoreTweenBuilder& FromVisibility(ESlateVisibility Value);
	FCoreTweenBuilder& ToVisibility(ESlateVisibility Value);
	FCoreTweenBuilder& FromMaxDesiredHeight(float Value);
	FCoreTweenBuilder& ToMaxDesiredHeight(float Value);

	// ── Configuration ───────────────────────────────────────────────

	FCoreTweenBuilder& Easing(ECoreTweenEasingType InType, TOptional<float> InParam = {});
	FCoreTweenBuilder& ToReset();
	FCoreTweenBuilder& SetDelay(float Seconds);
	FCoreTweenBuilder& SetLoops(int32 Count);
	FCoreTweenBuilder& SetPingPong(bool bInPingPong);
	FCoreTweenBuilder& SetTimeSource(ECoreTweenTimeSource Source);

	// ── Callbacks ───────────────────────────────────────────────────

	FCoreTweenBuilder& OnStart(const FCoreTweenSignature& InDelegate);
	FCoreTweenBuilder& OnComplete(const FCoreTweenSignature& InDelegate);

	// ── Terminal — returns the awaitable task ────────────────────────

	AsyncFlow::TTask<void> Run(UObject* WorldContext);

private:
	/** Apply all set properties at the given eased alpha. */
	void ApplyAll(float EasedAlpha);

	/** Read current values from target for properties without explicit start. */
	void BeginAllProperties();

	TSharedPtr<ICoreTweenTarget> Target;
	TWeakObjectPtr<UWidget> WidgetPtr;

	float Duration = 1.0f;
	float Delay = 0.0f;
	bool bAdditive = false;

	ECoreTweenEasingType EasingType = ECoreTweenEasingType::Linear;
	TOptional<float> EasingParam;

	int32 LoopCount = 1;
	bool bPingPong = false;
	ECoreTweenTimeSource TimeSource = ECoreTweenTimeSource::Unpaused;

	TCoreTweenProp<FVector2D> TranslationProp;
	TCoreTweenProp<FVector2D> ScaleProp;
	TCoreTweenProp<FLinearColor> ColorProp;
	TCoreTweenProp<float> OpacityProp;
	TCoreTweenProp<float> RotationProp;
	TCoreTweenProp<FVector2D> CanvasPositionProp;
	TCoreTweenProp<FVector4> PaddingProp;
	TCoreTweenInstantProp<ESlateVisibility> VisibilityProp;
	TCoreTweenProp<float> MaxDesiredHeightProp;

	FCoreTweenSignature OnStartDelegate;
	FCoreTweenSignature OnCompleteDelegate;
};

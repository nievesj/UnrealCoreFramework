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

#include "CoreTween.h"

#include "CoreTweenWorldSubsystem.h"
#include "Targets/WidgetTweenTarget.h"
#include "Components/Widget.h"
#include "Engine/World.h"

// ============================================================================
// UCoreTween — static factory + management (delegates to per-world subsystem)
// ============================================================================

UCoreTweenWorldSubsystem* UCoreTween::GetSubsystem(UObject* WorldContext)
{
	if (!WorldContext)
	{
		return nullptr;
	}
	UWorld* World = WorldContext->GetWorld();
	if (!World)
	{
		return nullptr;
	}
	return World->GetSubsystem<UCoreTweenWorldSubsystem>();
}

FCoreTweenBuilder UCoreTween::Create(UWidget* Widget, const float Duration, const float Delay, const bool bAdditive)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(Widget);
	if (Subsystem && !bAdditive)
	{
		Subsystem->Clear(Widget);
	}

	TSharedPtr<FWidgetTweenTarget> Target = MakeShared<FWidgetTweenTarget>(Widget);
	return FCoreTweenBuilder(Target, Widget, Duration, Delay, bAdditive);
}

int32 UCoreTween::Clear(UWidget* Widget)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(Widget);
	if (!Subsystem)
	{
		return 0;
	}
	return Subsystem->Clear(Widget);
}

bool UCoreTween::GetIsTweening(UWidget* Widget)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(Widget);
	if (!Subsystem)
	{
		return false;
	}
	return Subsystem->GetIsTweening(Widget);
}

void UCoreTween::CompleteAll(UObject* WorldContext)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(WorldContext);
	if (!Subsystem)
	{
		return;
	}
	Subsystem->CompleteAll();
}

void UCoreTween::RegisterTweenState(UObject* WorldContext, TSharedPtr<FCoreTweenState> State)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(WorldContext);
	if (!Subsystem)
	{
		return;
	}
	Subsystem->RegisterTweenState(MoveTemp(State));
}

TArray<TSharedPtr<FCoreTweenState>>* UCoreTween::GetActiveTweenStates(UObject* WorldContext)
{
	UCoreTweenWorldSubsystem* Subsystem = GetSubsystem(WorldContext);
	if (!Subsystem)
	{
		return nullptr;
	}
	return &Subsystem->GetActiveTweenStates();
}

// ============================================================================
// Blueprint function library
// ============================================================================

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::CreateTween(
	UWidget* Widget, const float Duration, const float Delay, const bool bAdditive)
{
	UCoreTweenParamChain* Chain = NewObject<UCoreTweenParamChain>();
	if (Chain)
	{
		Chain->Builder = UCoreTween::Create(Widget, Duration, Delay, bAdditive);
	}
	return Chain;
}

void UCoreTweenBlueprintFunctionLibrary::RunTween(UCoreTweenParamChain* Params, UObject* WorldContext)
{
	if (!Params)
	{
		return;
	}
	AsyncFlow::TTask<void> Task = Params->Builder.Run(WorldContext);
	Task.Start();

	// Store the task in the TweenState so the coroutine survives until completion.
	// The TweenState is the last one registered (most recent entry in ActiveTweenStates).
	TArray<TSharedPtr<FCoreTweenState>>* States = UCoreTween::GetActiveTweenStates(WorldContext);
	if (States && States->Num() > 0)
	{
		States->Last()->RunningTask = MoveTemp(Task);
	}

	Params->ConditionalBeginDestroy();
}

// ── Property setters ────────────────────────────────────────────────

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToTranslation(UCoreTweenParamChain* Chain, const FVector2D Target)
{
	if (Chain)
	{
		Chain->Builder.ToTranslation(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromTranslation(UCoreTweenParamChain* Chain, const FVector2D Start)
{
	if (Chain)
	{
		Chain->Builder.FromTranslation(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToScale(UCoreTweenParamChain* Chain, const FVector2D Target)
{
	if (Chain)
	{
		Chain->Builder.ToScale(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromScale(UCoreTweenParamChain* Chain, const FVector2D Start)
{
	if (Chain)
	{
		Chain->Builder.FromScale(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToOpacity(UCoreTweenParamChain* Chain, const float Target)
{
	if (Chain)
	{
		Chain->Builder.ToOpacity(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromOpacity(UCoreTweenParamChain* Chain, const float Start)
{
	if (Chain)
	{
		Chain->Builder.FromOpacity(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToColor(UCoreTweenParamChain* Chain, const FLinearColor Target)
{
	if (Chain)
	{
		Chain->Builder.ToColor(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromColor(UCoreTweenParamChain* Chain, const FLinearColor Start)
{
	if (Chain)
	{
		Chain->Builder.FromColor(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToRotation(UCoreTweenParamChain* Chain, const float Target)
{
	if (Chain)
	{
		Chain->Builder.ToRotation(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromRotation(UCoreTweenParamChain* Chain, const float Start)
{
	if (Chain)
	{
		Chain->Builder.FromRotation(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToCanvasPosition(UCoreTweenParamChain* Chain, const FVector2D Target)
{
	if (Chain)
	{
		Chain->Builder.ToCanvasPosition(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromCanvasPosition(UCoreTweenParamChain* Chain, const FVector2D Start)
{
	if (Chain)
	{
		Chain->Builder.FromCanvasPosition(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToPadding(UCoreTweenParamChain* Chain, const FMargin Target)
{
	if (Chain)
	{
		Chain->Builder.ToPadding(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromPadding(UCoreTweenParamChain* Chain, const FMargin Start)
{
	if (Chain)
	{
		Chain->Builder.FromPadding(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToVisibility(UCoreTweenParamChain* Chain, const ESlateVisibility Target)
{
	if (Chain)
	{
		Chain->Builder.ToVisibility(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromVisibility(UCoreTweenParamChain* Chain, const ESlateVisibility Start)
{
	if (Chain)
	{
		Chain->Builder.FromVisibility(Start);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToMaxDesiredHeight(UCoreTweenParamChain* Chain, const float Target)
{
	if (Chain)
	{
		Chain->Builder.ToMaxDesiredHeight(Target);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromMaxDesiredHeight(UCoreTweenParamChain* Chain, const float Start)
{
	if (Chain)
	{
		Chain->Builder.FromMaxDesiredHeight(Start);
	}
	return Chain;
}

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetEasing(UCoreTweenParamChain* Chain, const ECoreTweenEasingType Easing, const float EasingParam)
{
	if (Chain)
	{
		TOptional<float> Param;
		if (EasingParam != 0.0f)
		{
			Param = EasingParam;
		}
		Chain->Builder.Easing(Easing, Param);
	}
	return Chain;
}

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetLoops(UCoreTweenParamChain* Chain, const int32 LoopCount)
{
	if (Chain)
	{
		Chain->Builder.SetLoops(LoopCount);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetPingPong(UCoreTweenParamChain* Chain, const bool bPingPong)
{
	if (Chain)
	{
		Chain->Builder.SetPingPong(bPingPong);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetTimeSource(UCoreTweenParamChain* Chain, const ECoreTweenTimeSource TimeSource)
{
	if (Chain)
	{
		Chain->Builder.SetTimeSource(TimeSource);
	}
	return Chain;
}
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToReset(UCoreTweenParamChain* Chain)
{
	if (Chain)
	{
		Chain->Builder.ToReset();
	}
	return Chain;
}

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::OnStart(UCoreTweenParamChain* Chain, const FCoreTweenBPSignature& Callback)
{
	if (Chain)
	{
		FCoreTweenBPSignature CapturedCallback = Callback;
		Chain->Builder.OnStart(FCoreTweenSignature::CreateLambda([CapturedCallback]() {
			const_cast<FCoreTweenBPSignature&>(CapturedCallback).ExecuteIfBound();
		}));
	}
	return Chain;
}

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::OnComplete(UCoreTweenParamChain* Chain, const FCoreTweenBPSignature& Callback)
{
	if (Chain)
	{
		FCoreTweenBPSignature CapturedCallback = Callback;
		Chain->Builder.OnComplete(FCoreTweenSignature::CreateLambda([CapturedCallback]() {
			const_cast<FCoreTweenBPSignature&>(CapturedCallback).ExecuteIfBound();
		}));
	}
	return Chain;
}

// ── Management ──────────────────────────────────────────────────────

void UCoreTweenBlueprintFunctionLibrary::ClearTweens(UWidget* Widget)
{
	UCoreTween::Clear(Widget);
}

bool UCoreTweenBlueprintFunctionLibrary::IsTweening(UWidget* Widget)
{
	return UCoreTween::GetIsTweening(Widget);
}

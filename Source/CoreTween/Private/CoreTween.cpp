#include "CoreTween.h"

#include "Targets/WidgetTweenTarget.h"
#include "Components/Widget.h"

// Static member initialization
TArray<TSharedPtr<FCoreTweenState>> UCoreTween::ActiveTweenStates;

// ============================================================================
// UCoreTween — static factory + management
// ============================================================================

FCoreTweenBuilder UCoreTween::Create(UWidget* Widget, const float Duration, const float Delay, const bool bAdditive)
{
	PruneFinished();

	if (!bAdditive)
	{
		Clear(Widget);
	}

	TSharedPtr<FWidgetTweenTarget> Target = MakeShared<FWidgetTweenTarget>(Widget);
	return FCoreTweenBuilder(Target, Widget, Duration, Delay, bAdditive);
}

int32 UCoreTween::Clear(UWidget* Widget)
{
	int32 NumCancelled = 0;
	for (int32 Idx = ActiveTweenStates.Num() - 1; Idx >= 0; --Idx)
	{
		TSharedPtr<FCoreTweenState>& State = ActiveTweenStates[Idx];
		if (State->Widget.Get() == Widget)
		{
			if (State->FlowState.IsValid())
			{
				State->FlowState->Cancel();
			}
			State->bFinished.store(true, std::memory_order_release);
			ActiveTweenStates.RemoveAtSwap(Idx);
			NumCancelled++;
		}
	}
	return NumCancelled;
}

bool UCoreTween::GetIsTweening(UWidget* Widget)
{
	PruneFinished();
	for (const TSharedPtr<FCoreTweenState>& State : ActiveTweenStates)
	{
		if (State->Widget.Get() == Widget && !State->bFinished.load(std::memory_order_acquire))
		{
			return true;
		}
	}
	return false;
}

void UCoreTween::CompleteAll()
{
	for (const TSharedPtr<FCoreTweenState>& State : ActiveTweenStates)
	{
		if (!State->bFinished.load(std::memory_order_acquire))
		{
			State->bForceComplete.store(true, std::memory_order_release);
		}
	}
}

void UCoreTween::RegisterTweenState(TSharedPtr<FCoreTweenState> State)
{
	ActiveTweenStates.Add(MoveTemp(State));
}

void UCoreTween::PruneFinished()
{
	ActiveTweenStates.RemoveAll([](const TSharedPtr<FCoreTweenState>& State)
	{
		return State->bFinished.load(std::memory_order_acquire);
	});
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
		Chain->SetFlags(RF_MarkAsRootSet);
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
	Params->ClearFlags(RF_MarkAsRootSet);
	Params->ConditionalBeginDestroy();
}

// ── Property setters ────────────────────────────────────────────────

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToTranslation(UCoreTweenParamChain* Chain, const FVector2D Target) { if (Chain) { Chain->Builder.ToTranslation(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromTranslation(UCoreTweenParamChain* Chain, const FVector2D Start) { if (Chain) { Chain->Builder.FromTranslation(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToScale(UCoreTweenParamChain* Chain, const FVector2D Target) { if (Chain) { Chain->Builder.ToScale(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromScale(UCoreTweenParamChain* Chain, const FVector2D Start) { if (Chain) { Chain->Builder.FromScale(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToOpacity(UCoreTweenParamChain* Chain, const float Target) { if (Chain) { Chain->Builder.ToOpacity(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromOpacity(UCoreTweenParamChain* Chain, const float Start) { if (Chain) { Chain->Builder.FromOpacity(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToColor(UCoreTweenParamChain* Chain, const FLinearColor Target) { if (Chain) { Chain->Builder.ToColor(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromColor(UCoreTweenParamChain* Chain, const FLinearColor Start) { if (Chain) { Chain->Builder.FromColor(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToRotation(UCoreTweenParamChain* Chain, const float Target) { if (Chain) { Chain->Builder.ToRotation(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromRotation(UCoreTweenParamChain* Chain, const float Start) { if (Chain) { Chain->Builder.FromRotation(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToCanvasPosition(UCoreTweenParamChain* Chain, const FVector2D Target) { if (Chain) { Chain->Builder.ToCanvasPosition(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromCanvasPosition(UCoreTweenParamChain* Chain, const FVector2D Start) { if (Chain) { Chain->Builder.FromCanvasPosition(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToPadding(UCoreTweenParamChain* Chain, const FMargin Target) { if (Chain) { Chain->Builder.ToPadding(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromPadding(UCoreTweenParamChain* Chain, const FMargin Start) { if (Chain) { Chain->Builder.FromPadding(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToVisibility(UCoreTweenParamChain* Chain, const ESlateVisibility Target) { if (Chain) { Chain->Builder.ToVisibility(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromVisibility(UCoreTweenParamChain* Chain, const ESlateVisibility Start) { if (Chain) { Chain->Builder.FromVisibility(Start); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToMaxDesiredHeight(UCoreTweenParamChain* Chain, const float Target) { if (Chain) { Chain->Builder.ToMaxDesiredHeight(Target); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::FromMaxDesiredHeight(UCoreTweenParamChain* Chain, const float Start) { if (Chain) { Chain->Builder.FromMaxDesiredHeight(Start); } return Chain; }

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

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetLoops(UCoreTweenParamChain* Chain, const int32 LoopCount) { if (Chain) { Chain->Builder.SetLoops(LoopCount); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetPingPong(UCoreTweenParamChain* Chain, const bool bPingPong) { if (Chain) { Chain->Builder.SetPingPong(bPingPong); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::SetTimeSource(UCoreTweenParamChain* Chain, const ECoreTweenTimeSource TimeSource) { if (Chain) { Chain->Builder.SetTimeSource(TimeSource); } return Chain; }
UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::ToReset(UCoreTweenParamChain* Chain) { if (Chain) { Chain->Builder.ToReset(); } return Chain; }

UCoreTweenParamChain* UCoreTweenBlueprintFunctionLibrary::OnStart(UCoreTweenParamChain* Chain, const FCoreTweenBPSignature& Callback)
{
	if (Chain)
	{
		FCoreTweenBPSignature CapturedCallback = Callback;
		Chain->Builder.OnStart(FCoreTweenSignature::CreateLambda([CapturedCallback]()
		{
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
		Chain->Builder.OnComplete(FCoreTweenSignature::CreateLambda([CapturedCallback]()
		{
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


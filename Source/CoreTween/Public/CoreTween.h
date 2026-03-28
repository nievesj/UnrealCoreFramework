#pragma once

#include "CoreTweenBuilder.h"
#include "CoreTweenEasing.h"
#include "CoreTweenTypes.h"
#include "UObject/Object.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SharedPointer.h"

#include "CoreTween.generated.h"

class UWidget;
class UCoreTweenWorldSubsystem;

/**
 * Static factory and management API for CoreTween.
 * Create() returns a builder by value; Run() on the builder launches
 * the coroutine and returns a TTask<void>.
 *
 * Tween state is stored per-world via UCoreTweenWorldSubsystem so that
 * PIE with multiple worlds keeps tweens isolated.
 */
UCLASS()
class CORETWEEN_API UCoreTween : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Create a tween builder targeting any UWidget.
	 * The widget's world is used to look up the per-world subsystem.
	 *
	 * @param Widget     The widget to animate.
	 * @param Duration   Animation duration in seconds.
	 * @param Delay      Delay before animation starts.
	 * @param bAdditive  If false (default), existing tweens on this widget are cancelled first.
	 * @return           Builder by value — chain setters, then call Run().
	 */
	static FCoreTweenBuilder Create(UWidget* Widget, float Duration = 1.0f, float Delay = 0.0f, bool bAdditive = false);

	/** Cancel all active tweens on this widget. Derives world from Widget. */
	static int32 Clear(UWidget* Widget);

	/** @return true if any tween is actively running on this widget. Derives world from Widget. */
	static bool GetIsTweening(UWidget* Widget);

	/** Force-complete all active tweens in the given world context. */
	static void CompleteAll(UObject* WorldContext);

	/** Internal — called by FCoreTweenBuilder::Run() to register shared state. */
	static void RegisterTweenState(UObject* WorldContext, TSharedPtr<FCoreTweenState> State);

	/** Accessor for the active tween states array in the given world context. */
	static TArray<TSharedPtr<FCoreTweenState>>* GetActiveTweenStates(UObject* WorldContext);

private:
	/** Resolve the per-world subsystem from any UObject with a world. */
	static UCoreTweenWorldSubsystem* GetSubsystem(UObject* WorldContext);
};

// ============================================================================
// Blueprint param chain — mirrors UBUIParamChain
// ============================================================================

UCLASS(BlueprintType)
class CORETWEEN_API UCoreTweenParamChain : public UObject
{
	GENERATED_BODY()

public:
	/** Internally holds the builder state. Consumed by RunTween. */
	FCoreTweenBuilder Builder;
};

// ============================================================================
// Blueprint function library — all nodes accept plain UWidget*
// ============================================================================

UCLASS()
class CORETWEEN_API UCoreTweenBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* CreateTween(UWidget* Widget, float Duration = 1.0f, float Delay = 0.0f, bool bAdditive = false);

	UFUNCTION(BlueprintCallable, Category = "CoreTween", meta = (WorldContext = "WorldContext"))
	static void RunTween(UCoreTweenParamChain* Params, UObject* WorldContext);

	// ── Property setters ────────────────────────────────────────────

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToTranslation(UCoreTweenParamChain* Chain, FVector2D Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromTranslation(UCoreTweenParamChain* Chain, FVector2D Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToScale(UCoreTweenParamChain* Chain, FVector2D Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromScale(UCoreTweenParamChain* Chain, FVector2D Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToOpacity(UCoreTweenParamChain* Chain, float Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromOpacity(UCoreTweenParamChain* Chain, float Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToColor(UCoreTweenParamChain* Chain, FLinearColor Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromColor(UCoreTweenParamChain* Chain, FLinearColor Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToRotation(UCoreTweenParamChain* Chain, float Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromRotation(UCoreTweenParamChain* Chain, float Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToCanvasPosition(UCoreTweenParamChain* Chain, FVector2D Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromCanvasPosition(UCoreTweenParamChain* Chain, FVector2D Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToPadding(UCoreTweenParamChain* Chain, FMargin Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromPadding(UCoreTweenParamChain* Chain, FMargin Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToVisibility(UCoreTweenParamChain* Chain, ESlateVisibility Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromVisibility(UCoreTweenParamChain* Chain, ESlateVisibility Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToMaxDesiredHeight(UCoreTweenParamChain* Chain, float Target);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* FromMaxDesiredHeight(UCoreTweenParamChain* Chain, float Start);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* SetEasing(UCoreTweenParamChain* Chain, ECoreTweenEasingType Easing, float EasingParam = 0.0f);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* SetLoops(UCoreTweenParamChain* Chain, int32 LoopCount);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* SetPingPong(UCoreTweenParamChain* Chain, bool bPingPong);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* SetTimeSource(UCoreTweenParamChain* Chain, ECoreTweenTimeSource TimeSource);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* ToReset(UCoreTweenParamChain* Chain);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* OnStart(UCoreTweenParamChain* Chain, const FCoreTweenBPSignature& Callback);

	UFUNCTION(BlueprintPure, Category = "CoreTween")
	static UCoreTweenParamChain* OnComplete(UCoreTweenParamChain* Chain, const FCoreTweenBPSignature& Callback);

	// ── Management nodes ────────────────────────────────────────────

	UFUNCTION(BlueprintCallable, Category = "CoreTween")
	static void ClearTweens(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "CoreTween")
	static bool IsTweening(UWidget* Widget);
};


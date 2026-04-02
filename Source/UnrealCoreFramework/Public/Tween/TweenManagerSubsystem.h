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

#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "UI/UiCoreFrameworkTypes.h"
#include "AsyncFlowTask.h"

#include "TweenManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreTweenManager, Log, All);

class UCoreWidget;
class UWidget;

UCLASS()
class UNREALCOREFRAMEWORK_API UTweenManagerSubsystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Play a widget transition effect (fire-and-forget Blueprint entry point) */
	UFUNCTION(BlueprintCallable, Category = "WidgetAnimation")
	void PlayWidgetTransitionEffect(
		UCoreWidget* Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode TransitionMode);

	/**
	 * Play a widget transition effect as a coroutine.
	 * Resolves when the animation completes.
	 */
	AsyncFlow::TTask<void> PlayWidgetTransitionEffectTask(
		UCoreWidget* Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode TransitionMode);

	/** Play a preset animation */
	UFUNCTION(BlueprintCallable, Category = "WidgetAnimation")
	bool PlayPresetAnimation(
		UCoreWidget* Widget,
		const FName& PresetName,
		EWidgetTransitionMode TransitionMode);

	/** Register animation presets */
	void RegisterAnimationPresets(const TMap<FName, FWidgetTweenTransitionOptions>& Presets);

	/** Should animations be played based on performance settings */
	bool ShouldPlayAnimations() const;

protected:
	AsyncFlow::TTask<void> PlayScaleAnimationTask(
		UCoreWidget* Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode TransitionMode);

	AsyncFlow::TTask<void> PlayTranslationAnimationTask(
		UCoreWidget* Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode TransitionMode);

	AsyncFlow::TTask<void> PlayFadeAnimationTask(
		UCoreWidget* Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode TransitionMode);

	static void GetViewportTranslationVectors(
		EWidgetTranslationType TranslationType,
		FVector2D& OutStart,
		FVector2D& OutEnd);

	AsyncFlow::TTask<void> CreateAndPlayTweenTask(
		UCoreWidget* Widget,
		const FVector2D& StartScale,
		const FVector2D& EndScale,
		const FVector2D& StartTranslation,
		const FVector2D& EndTranslation,
		float StartOpacity,
		float EndOpacity,
		float Duration,
		ECoreTweenEasingType EasingType,
		EWidgetTransitionMode TransitionMode);

private:
	TMap<FName, FWidgetTweenTransitionOptions> AnimationPresets;

	/** Active animation task (needs next-frame deferral) */
	AsyncFlow::TTask<void> ActiveAnimationTask;
};

#pragma once

#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "UI/UiCoreFrameworkTypes.h"
#include "AsyncFlowTask.h"

#include "TweenManagerSubsystem.generated.h"

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
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

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
		UCoreWidget*		  Widget,
		const FName&		  PresetName,
		EWidgetTransitionMode TransitionMode);

	/** Register animation presets */
	void RegisterAnimationPresets(const TMap<FName, FWidgetTweenTransitionOptions>& Presets);

	/** Should animations be played based on performance settings */
	bool ShouldPlayAnimations() const;

protected:
	AsyncFlow::TTask<void> PlayScaleAnimationTask(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	AsyncFlow::TTask<void> PlayTranslationAnimationTask(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	AsyncFlow::TTask<void> PlayFadeAnimationTask(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	static void GetViewportTranslationVectors(
		EWidgetTranslationType TranslationType,
		FVector2D&			   OutStart,
		FVector2D&			   OutEnd);

	AsyncFlow::TTask<void> CreateAndPlayTweenTask(
		UCoreWidget*	 Widget,
		const FVector2D& StartScale, const FVector2D& EndScale,
		const FVector2D& StartTranslation, const FVector2D& EndTranslation,
		float StartOpacity, float EndOpacity, float Duration,
		ECoreTweenEasingType EasingType, EWidgetTransitionMode TransitionMode);

private:
	TMap<FName, FWidgetTweenTransitionOptions> AnimationPresets;

	/** Active animation task (needs next-frame deferral) */
	AsyncFlow::TTask<void> ActiveAnimationTask;
};


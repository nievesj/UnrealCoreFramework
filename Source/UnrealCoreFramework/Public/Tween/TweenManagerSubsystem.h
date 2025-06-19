#pragma once

#include "SubSystems/Base/CoreGameInstanceSubsystem.h"
#include "Tickable.h"
#include "UI/UiCoreFrameworkTypes.h"

#include "TweenManagerSubsystem.generated.h"

enum class EWidgetTranslationType : uint8;
class UCoreWidget;
enum class EBUIEasingType : uint8;
class UWidget;

UCLASS()
class UNREALCOREFRAMEWORK_API UTweenManagerSubsystem : public UCoreGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void	Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void	Deinitialize() override;
	virtual bool	IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual void	Tick(float DeltaTime) override;

	/** Play a widget transition effect */
	UFUNCTION(BlueprintCallable, Category = "WidgetAnimation")
	void PlayWidgetTransitionEffect(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

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
	void PlayScaleAnimation(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	void PlayTranslationAnimation(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	void PlayFadeAnimation(
		UCoreWidget*						 Widget,
		const FWidgetTweenTransitionOptions& TransitionOptions,
		EWidgetTransitionMode				 TransitionMode);

	static void GetViewportTranslationVectors(
		EWidgetTranslationType TranslationType,
		FVector2D&			   OutStart,
		FVector2D&			   OutEnd);

	void CreateAndPlayTween(
		UCoreWidget*	 Widget,
		const FVector2D& StartScale, const FVector2D& EndScale,
		const FVector2D& StartTranslation, const FVector2D& EndTranslation,
		float StartOpacity, float EndOpacity, float Duration,
		EBUIEasingType EasingType, EWidgetTransitionMode TransitionMode);

private:
	TMap<FName, FWidgetTweenTransitionOptions> AnimationPresets;
};
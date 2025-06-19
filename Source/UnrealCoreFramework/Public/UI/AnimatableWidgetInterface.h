#pragma once

#include "UiCoreFrameworkTypes.h"
#include "UObject/Interface.h"

#include "AnimatableWidgetInterface.generated.h"

UINTERFACE(BlueprintType)
class UNREALCOREFRAMEWORK_API UAnimatableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALCOREFRAMEWORK_API IAnimatableWidgetInterface
{
	GENERATED_BODY()

public:
	/** Returns the animation settings for this widget */
	virtual FCoreWidgetAnimationSettings GetWidgetAnimationSettings() { return FCoreWidgetAnimationSettings(); }

	/** Called when an animation begins playing on this widget */
	virtual void OnAnimationStarted(const EWidgetTransitionMode& TransitionMode) {}

	/** Called when an animation completes on this widget */
	virtual void OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode) {}

	/** Whether animations should be played for this widget */
	virtual bool ShouldPlayAnimations() const { return true; }
};
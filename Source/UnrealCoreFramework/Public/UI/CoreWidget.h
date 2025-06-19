#pragma once

#include "AnimatableWidgetInterface.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "UiCoreFrameworkTypes.h"

#include "CoreWidget.generated.h"

class UTweenManagerSubsystem;
class UWidgetAnimationHelper;
class UCanvasPanel;
class UUISubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreWidget, Log, All);

UCLASS(Abstract, Blueprintable, ClassGroup = UI, meta = (Category = "Core Framework UI", DisableNativeTick))
class UNREALCOREFRAMEWORK_API UCoreWidget : public UCommonActivatableWidget, public IAnimatableWidgetInterface
{
	GENERATED_BODY()

public:
	UUMGSequencePlayer* GetSequencePlayer(UWidgetAnimation* InAnimation);
	virtual void		NativePreConstruct() override;

	UFUNCTION(BlueprintCallable, Category = CoreWidget)
	void Show();

	UFUNCTION(BlueprintCallable, Category = CoreWidget)
	void Hide();

	UFUNCTION(BlueprintImplementableEvent, Category = CoreWidget)
	void OnShown();

	UFUNCTION(BlueprintImplementableEvent, Category = CoreWidget)
	void OnHidden();

	virtual FCoreWidgetAnimationSettings GetWidgetAnimationSettings() override
	{
		return CoreWidgetAnimationSettings;
	}

protected:
	void PlayTweenTransition(const FWidgetTweenTransitionOptions& TweenTransitionOptions, const EWidgetTransitionMode WidgetTransitionMode);
	void PlayWidgetAnimation(UWidgetAnimation* Anim, const FWidgetAnimationOptions& WidgetAnimationOptions, const EWidgetTransitionMode WidgetTransitionMode);

	/** Called when an animation begins playing on this widget */
	virtual void OnAnimationStarted(const EWidgetTransitionMode& TransitionMode) override;

	/** Called when an animation completes on this widget */
	virtual void OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode) override;

	/** Whether animations should be played for this widget */
	virtual bool ShouldPlayAnimations() const override { return true; }

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual void InternalShown();
	virtual void InternalHidden();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget)
	bool bDestroyOnDeactivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget)
	FCoreWidgetAnimationSettings CoreWidgetAnimationSettings;

	/** Helper object for managing animations */
	UPROPERTY(Transient)
	TObjectPtr<UTweenManagerSubsystem> TweenManagerSubsystem = nullptr;
};
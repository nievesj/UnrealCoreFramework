#pragma once

#include "CoreWidget.h"
#include "UiCoreFrameworkTypes.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

#include "CoreStackWidgetContainer.generated.h"

class UCoreWidget;
class UWidgetAnimationHelper;

/**
 * Container for stacking Core widgets with animation support
 * Handles transitions, animations, and widget lifecycle management
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UCoreStackWidgetContainer : public UCommonActivatableWidgetStack
{
	GENERATED_BODY()

public:
	UCoreStackWidgetContainer();

	/**
	 * Adds a widget to the stack with type-safety
	 * @param WidgetClass The class of widget to create and add
	 * @return The created widget, or nullptr if creation failed
	 */
	template <typename T = UCoreWidget>
	T* AddWidgetToStack(TSubclassOf<UCoreWidget> WidgetClass)
	{
		if (!IsValid(WidgetClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("AddWidgetToStack: Invalid widget class"));
			return nullptr;
		}

		if (!WidgetClass->IsChildOf<T>())
		{
			UE_LOG(LogTemp, Warning, TEXT("AddWidgetToStack: Widget class %s is not a child of %s"),
				*WidgetClass->GetName(), *T::StaticClass()->GetName());
			return nullptr;
		}

		T* CreatedWidget = AddWidget<T>(WidgetClass);
		if (CreatedWidget)
		{
			ActiveWidgets.Add(CreatedWidget);
		}
		return CreatedWidget;
	}

	/**
	 * Adds a widget to the stack with an entrance animation
	 * @param WidgetClass The class of widget to create and add
	 * @param AnimationPreset Name of the animation preset to use for entrance
	 * @return The created widget, or nullptr if creation failed
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	UCoreWidget* AddWidgetWithAnimation(TSubclassOf<UCoreWidget> WidgetClass, FName AnimationPreset);

	/**
	 * Removes a widget from the stack
	 * @param WidgetToRemove Widget to remove from the stack
	 * @param bAnimate Whether to play an exit animation
	 * @param AnimationPreset Name of the animation preset to use for exit (if animating)
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	void RemoveWidgetFromStack(UCoreWidget* WidgetToRemove, bool bAnimate = true, FName AnimationPreset = NAME_None);

	/**
	 * Gets the current top widget in the stack
	 * @return The top widget or nullptr if stack is empty
	 */
	UFUNCTION(BlueprintPure, Category = "UI|Stack")
	UCoreWidget* GetTopWidget() const;

	/**
	 * Clears all widgets from the stack
	 * @param bAnimate Whether to animate the removal
	 */
	UFUNCTION(BlueprintCallable, Category = "UI|Stack")
	void ClearWidgetStack(bool bAnimate = true);

protected:
	virtual void OnWidgetAddedToList(UCommonActivatableWidget& AddedWidget) override;

	/**
	 * Sets the animation settings used by this container
	 * @param WidgetAnimationSettings The animation settings to use
	 */
	void SetWidgetAnimationSettings(const FCoreWidgetAnimationSettings& WidgetAnimationSettings);

private:
	/** Currently active widgets in this stack */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UCoreWidget>> ActiveWidgets;

	/** Widgets pending removal after animation completes */
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<UCoreWidget>> PendingRemovalWidgets;

	/** Default animation settings if not specified */
	UPROPERTY(EditAnywhere, Category = "Animation")
	FCoreWidgetAnimationSettings DefaultAnimationSettings;

	/** Whether animations are enabled for this container */
	UPROPERTY(EditAnywhere, Category = "Animation")
	bool bAnimationsEnabled;
};
#pragma once

#include "Targets/CoreTweenTarget.h"
#include "UObject/WeakObjectPtrTemplates.h"

class UWidget;

/**
 * ICoreTweenTarget implementation for UWidget.
 * Reads and writes UMG widget properties — render transform, opacity,
 * color, visibility, canvas position, padding, and max desired height.
 *
 * Holds the widget via TWeakObjectPtr; IsTargetValid() returns false
 * if the widget has been garbage collected.
 */
class CORETWEEN_API FWidgetTweenTarget : public ICoreTweenTarget
{
public:
	explicit FWidgetTweenTarget(UWidget* InWidget);

	virtual bool IsTargetValid() const override;

	virtual FVector2D GetCurrentTranslation() const override;
	virtual FVector2D GetCurrentScale() const override;
	virtual float GetCurrentOpacity() const override;
	virtual FLinearColor GetCurrentColor() const override;
	virtual float GetCurrentRotation() const override;
	virtual FVector2D GetCurrentCanvasPosition() const override;
	virtual FVector4 GetCurrentPadding() const override;
	virtual ESlateVisibility GetCurrentVisibility() const override;
	virtual float GetCurrentMaxDesiredHeight() const override;

	virtual void ApplyTranslation(const FVector2D& Value) override;
	virtual void ApplyScale(const FVector2D& Value) override;
	virtual void ApplyOpacity(float Value) override;
	virtual void ApplyColor(const FLinearColor& Value) override;
	virtual void ApplyRotation(float Value) override;
	virtual void ApplyCanvasPosition(const FVector2D& Value) override;
	virtual void ApplyPadding(const FVector4& Value) override;
	virtual void ApplyVisibility(ESlateVisibility Value) override;
	virtual void ApplyMaxDesiredHeight(float Value) override;

	UWidget* GetWidget() const;

private:
	TWeakObjectPtr<UWidget> WeakWidget;
};


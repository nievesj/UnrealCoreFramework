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

#include "Targets/CoreTweenTarget.h"
#include "UObject/WeakObjectPtrTemplates.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreTweenTarget, Log, All);

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


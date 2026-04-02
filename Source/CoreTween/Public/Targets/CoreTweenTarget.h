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

#include "Math/Vector2D.h"
#include "Math/Vector4.h"
#include "Math/Color.h"
#include "Components/SlateWrapperTypes.h"

/**
 * Abstraction layer for tween property targets.
 * Decouples the interpolation engine from the concrete target type
 * (UWidget, AActor, etc.) so new target types can be added without
 * modifying the builder or runner.
 */
class ICoreTweenTarget
{
public:
	virtual ~ICoreTweenTarget() = default;

	/** @return true if the underlying target object is still alive. */
	virtual bool IsTargetValid() const = 0;

	// ── Read current values (for "from current" behavior) ───────────

	virtual FVector2D GetCurrentTranslation() const = 0;
	virtual FVector2D GetCurrentScale() const = 0;
	virtual float GetCurrentOpacity() const = 0;
	virtual FLinearColor GetCurrentColor() const = 0;
	virtual float GetCurrentRotation() const = 0;
	virtual FVector2D GetCurrentCanvasPosition() const = 0;
	virtual FVector4 GetCurrentPadding() const = 0;
	virtual ESlateVisibility GetCurrentVisibility() const = 0;
	virtual float GetCurrentMaxDesiredHeight() const = 0;

	// ── Apply interpolated values ───────────────────────────────────

	virtual void ApplyTranslation(const FVector2D& Value) = 0;
	virtual void ApplyScale(const FVector2D& Value) = 0;
	virtual void ApplyOpacity(float Value) = 0;
	virtual void ApplyColor(const FLinearColor& Value) = 0;
	virtual void ApplyRotation(float Value) = 0;
	virtual void ApplyCanvasPosition(const FVector2D& Value) = 0;
	virtual void ApplyPadding(const FVector4& Value) = 0;
	virtual void ApplyVisibility(ESlateVisibility Value) = 0;
	virtual void ApplyMaxDesiredHeight(float Value) = 0;

	// ── Actor property access (future — not yet implemented) ────────
	// When FActorTweenTarget is built, add:
	//   virtual FVector    GetCurrentLocation() const = 0;
	//   virtual FRotator   GetCurrentActorRotation() const = 0;
	//   virtual void ApplyLocation(const FVector& Value) = 0;
	//   virtual void ApplyActorRotation(const FRotator& Value) = 0;
};

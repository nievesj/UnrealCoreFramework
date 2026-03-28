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


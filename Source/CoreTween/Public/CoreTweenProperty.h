#pragma once

#include "Math/UnrealMathUtility.h"

/**
 * Interpolating property template. Tracks start, target, and current values
 * for a single tweened property. Lerps between Start and Target based on
 * the eased alpha provided each frame.
 *
 * @tparam T  Value type (must support FMath::Lerp — FVector2D, float, FLinearColor, FVector4).
 */
template <typename T>
class TCoreTweenProp
{
public:
	bool bHasStart = false;
	bool bHasTarget = false;
	T StartValue{};
	T TargetValue{};
	T CurrentValue{};
	bool bIsFirstTime = true;

	bool IsSet() const
	{
		return bHasStart || bHasTarget;
	}

	void SetStart(T InStart)
	{
		bHasStart = true;
		StartValue = InStart;
		CurrentValue = StartValue;
	}

	void SetTarget(T InTarget)
	{
		bHasTarget = true;
		TargetValue = InTarget;
	}

	/** Called at tween begin — fills in StartValue from the widget's current state if no explicit start was set. */
	void OnBegin(T InCurrentValue)
	{
		if (!bHasStart)
		{
			StartValue = InCurrentValue;
			CurrentValue = InCurrentValue;
		}
		if (!bHasTarget)
		{
			TargetValue = InCurrentValue;
		}
	}

	/**
	 * Update the current value based on eased alpha.
	 * @return true if the value changed (or first frame), false if unchanged.
	 */
	bool Update(float EasedAlpha)
	{
		const T OldValue = CurrentValue;
		CurrentValue = FMath::Lerp<T>(StartValue, TargetValue, EasedAlpha);
		const bool bShouldUpdate = bIsFirstTime || (CurrentValue != OldValue);
		bIsFirstTime = false;
		return bShouldUpdate;
	}
};

/**
 * Discrete (snap) property template. For properties like ESlateVisibility
 * that switch at alpha thresholds rather than interpolating continuously.
 *
 * @tparam T  Discrete value type (enum, bool, etc.).
 */
template <typename T>
class TCoreTweenInstantProp
{
public:
	bool bHasStart = false;
	bool bHasTarget = false;
	T StartValue{};
	T TargetValue{};
	T CurrentValue{};
	bool bIsFirstTime = true;

	bool IsSet() const
	{
		return bHasStart || bHasTarget;
	}

	void SetStart(T InStart)
	{
		bHasStart = true;
		StartValue = InStart;
		CurrentValue = StartValue;
	}

	void SetTarget(T InTarget)
	{
		bHasTarget = true;
		TargetValue = InTarget;
	}

	void OnBegin(T InCurrentValue)
	{
		if (!bHasStart)
		{
			StartValue = InCurrentValue;
			CurrentValue = InCurrentValue;
		}
		if (!bHasTarget)
		{
			TargetValue = InCurrentValue;
		}
	}

	/**
	 * Snap to target when alpha >= 1, otherwise hold start.
	 * @return true if the value changed (or first frame).
	 */
	bool Update(float EasedAlpha)
	{
		const T OldValue = CurrentValue;
		if (EasedAlpha >= 1.0f && bHasTarget)
		{
			CurrentValue = TargetValue;
		}
		else
		{
			CurrentValue = StartValue;
		}
		const bool bShouldChange = bIsFirstTime || (OldValue != CurrentValue);
		bIsFirstTime = false;
		return bShouldChange;
	}
};


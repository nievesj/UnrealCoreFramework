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


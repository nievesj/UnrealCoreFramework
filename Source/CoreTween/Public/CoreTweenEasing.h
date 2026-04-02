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

#include "HAL/Platform.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/Optional.h"
#include "UObject/ObjectMacros.h"

#include "CoreTweenEasing.generated.h"

UENUM(BlueprintType)
enum class ECoreTweenEasingType : uint8
{
	Linear,

	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,

	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,

	EaseInQuart,
	EaseOutQuart,
	EaseInOutQuart,

	EaseInQuint,
	EaseOutQuint,
	EaseInOutQuint,

	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,

	EaseInSine,
	EaseOutSine,
	EaseInOutSine,

	EaseInCirc,
	EaseOutCirc,
	EaseInOutCirc,

	EaseInBack,
	EaseOutBack,
	EaseInOutBack,

	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,

	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce,

	Spring
};

/**
 * Pure-function easing library. All functions accept normalized alpha [0,1]
 * and return eased alpha (some may overshoot for Back/Elastic/Spring).
 */
struct CORETWEEN_API FCoreTweenEasing
{
	/**
	 * Dispatch to the appropriate easing function.
	 *
	 * @param Type   Easing curve.
	 * @param Alpha  Normalized progress [0, 1].
	 * @param Param  Optional parameter (overshoot for Back, amplitude for Elastic, damping for Spring).
	 * @return       Eased alpha value.
	 */
	static float Ease(ECoreTweenEasingType Type, float Alpha, TOptional<float> Param = {});

	// ── Individual easing functions ──────────────────────────────────

	static float Linear(float Alpha);

	static float InQuad(float Alpha);
	static float OutQuad(float Alpha);
	static float InOutQuad(float Alpha);

	static float InCubic(float Alpha);
	static float OutCubic(float Alpha);
	static float InOutCubic(float Alpha);

	static float InQuart(float Alpha);
	static float OutQuart(float Alpha);
	static float InOutQuart(float Alpha);

	static float InQuint(float Alpha);
	static float OutQuint(float Alpha);
	static float InOutQuint(float Alpha);

	static float InExpo(float Alpha);
	static float OutExpo(float Alpha);
	static float InOutExpo(float Alpha);

	static float InSine(float Alpha);
	static float OutSine(float Alpha);
	static float InOutSine(float Alpha);

	static float InCirc(float Alpha);
	static float OutCirc(float Alpha);
	static float InOutCirc(float Alpha);

	static float InBack(float Alpha, float Overshoot = 1.70158f);
	static float OutBack(float Alpha, float Overshoot = 1.70158f);
	static float InOutBack(float Alpha, float Overshoot = 1.70158f);

	static float InElastic(float Alpha, float Amplitude = 1.0f);
	static float OutElastic(float Alpha, float Amplitude = 1.0f);
	static float InOutElastic(float Alpha, float Amplitude = 1.0f);

	static float InBounce(float Alpha);
	static float OutBounce(float Alpha);
	static float InOutBounce(float Alpha);

	static float SpringEase(float Alpha, float Damping = 0.5f);
};

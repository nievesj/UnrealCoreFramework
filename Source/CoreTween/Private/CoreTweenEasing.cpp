#include "CoreTweenEasing.h"

#include "Math/UnrealMathUtility.h"

float FCoreTweenEasing::Ease(const ECoreTweenEasingType Type, const float Alpha, const TOptional<float> Param)
{
	switch (Type)
	{
		case ECoreTweenEasingType::Linear:        return Linear(Alpha);

		case ECoreTweenEasingType::EaseInQuad:    return InQuad(Alpha);
		case ECoreTweenEasingType::EaseOutQuad:   return OutQuad(Alpha);
		case ECoreTweenEasingType::EaseInOutQuad: return InOutQuad(Alpha);

		case ECoreTweenEasingType::EaseInCubic:    return InCubic(Alpha);
		case ECoreTweenEasingType::EaseOutCubic:   return OutCubic(Alpha);
		case ECoreTweenEasingType::EaseInOutCubic: return InOutCubic(Alpha);

		case ECoreTweenEasingType::EaseInQuart:    return InQuart(Alpha);
		case ECoreTweenEasingType::EaseOutQuart:   return OutQuart(Alpha);
		case ECoreTweenEasingType::EaseInOutQuart: return InOutQuart(Alpha);

		case ECoreTweenEasingType::EaseInQuint:    return InQuint(Alpha);
		case ECoreTweenEasingType::EaseOutQuint:   return OutQuint(Alpha);
		case ECoreTweenEasingType::EaseInOutQuint: return InOutQuint(Alpha);

		case ECoreTweenEasingType::EaseInExpo:    return InExpo(Alpha);
		case ECoreTweenEasingType::EaseOutExpo:   return OutExpo(Alpha);
		case ECoreTweenEasingType::EaseInOutExpo: return InOutExpo(Alpha);

		case ECoreTweenEasingType::EaseInSine:    return InSine(Alpha);
		case ECoreTweenEasingType::EaseOutSine:   return OutSine(Alpha);
		case ECoreTweenEasingType::EaseInOutSine: return InOutSine(Alpha);

		case ECoreTweenEasingType::EaseInCirc:    return InCirc(Alpha);
		case ECoreTweenEasingType::EaseOutCirc:   return OutCirc(Alpha);
		case ECoreTweenEasingType::EaseInOutCirc: return InOutCirc(Alpha);

		case ECoreTweenEasingType::EaseInBack:    return InBack(Alpha, Param.Get(1.70158f));
		case ECoreTweenEasingType::EaseOutBack:   return OutBack(Alpha, Param.Get(1.70158f));
		case ECoreTweenEasingType::EaseInOutBack: return InOutBack(Alpha, Param.Get(1.70158f));

		case ECoreTweenEasingType::EaseInElastic:    return InElastic(Alpha, Param.Get(1.0f));
		case ECoreTweenEasingType::EaseOutElastic:   return OutElastic(Alpha, Param.Get(1.0f));
		case ECoreTweenEasingType::EaseInOutElastic: return InOutElastic(Alpha, Param.Get(1.0f));

		case ECoreTweenEasingType::EaseInBounce:    return InBounce(Alpha);
		case ECoreTweenEasingType::EaseOutBounce:   return OutBounce(Alpha);
		case ECoreTweenEasingType::EaseInOutBounce: return InOutBounce(Alpha);

		case ECoreTweenEasingType::Spring: return SpringEase(Alpha, Param.Get(0.5f));

		default:
			return Alpha;
	}
}

// ── Linear ──────────────────────────────────────────────────────────

float FCoreTweenEasing::Linear(const float Alpha)
{
	return Alpha;
}

// ── Quad ────────────────────────────────────────────────────────────

float FCoreTweenEasing::InQuad(const float Alpha)
{
	return Alpha * Alpha;
}

float FCoreTweenEasing::OutQuad(const float Alpha)
{
	return -Alpha * (Alpha - 2.0f);
}

float FCoreTweenEasing::InOutQuad(const float Alpha)
{
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * T * T;
	}
	const float Shifted = T - 1.0f;
	return -0.5f * (Shifted * (Shifted - 2.0f) - 1.0f);
}

// ── Cubic ───────────────────────────────────────────────────────────

float FCoreTweenEasing::InCubic(const float Alpha)
{
	return Alpha * Alpha * Alpha;
}

float FCoreTweenEasing::OutCubic(const float Alpha)
{
	const float Shifted = Alpha - 1.0f;
	return Shifted * Shifted * Shifted + 1.0f;
}

float FCoreTweenEasing::InOutCubic(const float Alpha)
{
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * T * T * T;
	}
	const float Shifted = T - 2.0f;
	return 0.5f * (Shifted * Shifted * Shifted + 2.0f);
}

// ── Quart ───────────────────────────────────────────────────────────

float FCoreTweenEasing::InQuart(const float Alpha)
{
	return Alpha * Alpha * Alpha * Alpha;
}

float FCoreTweenEasing::OutQuart(const float Alpha)
{
	const float Shifted = Alpha - 1.0f;
	return -(Shifted * Shifted * Shifted * Shifted - 1.0f);
}

float FCoreTweenEasing::InOutQuart(const float Alpha)
{
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * T * T * T * T;
	}
	const float Shifted = T - 2.0f;
	return -0.5f * (Shifted * Shifted * Shifted * Shifted - 2.0f);
}

// ── Quint ───────────────────────────────────────────────────────────

float FCoreTweenEasing::InQuint(const float Alpha)
{
	return Alpha * Alpha * Alpha * Alpha * Alpha;
}

float FCoreTweenEasing::OutQuint(const float Alpha)
{
	const float Shifted = Alpha - 1.0f;
	return Shifted * Shifted * Shifted * Shifted * Shifted + 1.0f;
}

float FCoreTweenEasing::InOutQuint(const float Alpha)
{
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * T * T * T * T * T;
	}
	const float Shifted = T - 2.0f;
	return 0.5f * (Shifted * Shifted * Shifted * Shifted * Shifted + 2.0f);
}

// ── Expo ────────────────────────────────────────────────────────────

float FCoreTweenEasing::InExpo(const float Alpha)
{
	return (Alpha == 0.0f) ? 0.0f : FMath::Pow(2.0f, 10.0f * (Alpha - 1.0f));
}

float FCoreTweenEasing::OutExpo(const float Alpha)
{
	return (Alpha == 1.0f) ? 1.0f : (-FMath::Pow(2.0f, -10.0f * Alpha) + 1.0f);
}

float FCoreTweenEasing::InOutExpo(const float Alpha)
{
	if (Alpha == 0.0f)
	{
		return 0.0f;
	}
	if (Alpha == 1.0f)
	{
		return 1.0f;
	}
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * FMath::Pow(2.0f, 10.0f * (T - 1.0f));
	}
	return 0.5f * (-FMath::Pow(2.0f, -10.0f * (T - 1.0f)) + 2.0f);
}

// ── Sine ────────────────────────────────────────────────────────────

float FCoreTweenEasing::InSine(const float Alpha)
{
	return -FMath::Cos(Alpha * HALF_PI) + 1.0f;
}

float FCoreTweenEasing::OutSine(const float Alpha)
{
	return FMath::Sin(Alpha * HALF_PI);
}

float FCoreTweenEasing::InOutSine(const float Alpha)
{
	return -0.5f * (FMath::Cos(PI * Alpha) - 1.0f);
}

// ── Circ ────────────────────────────────────────────────────────────

float FCoreTweenEasing::InCirc(const float Alpha)
{
	return -(FMath::Sqrt(1.0f - Alpha * Alpha) - 1.0f);
}

float FCoreTweenEasing::OutCirc(const float Alpha)
{
	const float Shifted = Alpha - 1.0f;
	return FMath::Sqrt(1.0f - Shifted * Shifted);
}

float FCoreTweenEasing::InOutCirc(const float Alpha)
{
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return -0.5f * (FMath::Sqrt(1.0f - T * T) - 1.0f);
	}
	const float Shifted = T - 2.0f;
	return 0.5f * (FMath::Sqrt(1.0f - Shifted * Shifted) + 1.0f);
}

// ── Back ────────────────────────────────────────────────────────────

float FCoreTweenEasing::InBack(const float Alpha, const float Overshoot)
{
	return Alpha * Alpha * ((Overshoot + 1.0f) * Alpha - Overshoot);
}

float FCoreTweenEasing::OutBack(const float Alpha, const float Overshoot)
{
	const float Shifted = Alpha - 1.0f;
	return Shifted * Shifted * ((Overshoot + 1.0f) * Shifted + Overshoot) + 1.0f;
}

float FCoreTweenEasing::InOutBack(const float Alpha, const float Overshoot)
{
	const float S = Overshoot * 1.525f;
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		return 0.5f * (T * T * ((S + 1.0f) * T - S));
	}
	const float Shifted = T - 2.0f;
	return 0.5f * (Shifted * Shifted * ((S + 1.0f) * Shifted + S) + 2.0f);
}

// ── Elastic ─────────────────────────────────────────────────────────

float FCoreTweenEasing::InElastic(const float Alpha, const float Amplitude)
{
	if (Alpha == 0.0f || Alpha == 1.0f)
	{
		return Alpha;
	}
	const float Period = 0.3f;
	float S;
	float A = Amplitude;
	if (A < 1.0f)
	{
		A = 1.0f;
		S = Period / 4.0f;
	}
	else
	{
		S = Period / (2.0f * PI) * FMath::Asin(1.0f / A);
	}
	const float Shifted = Alpha - 1.0f;
	return -(A * FMath::Pow(2.0f, 10.0f * Shifted) * FMath::Sin((Shifted - S) * (2.0f * PI) / Period));
}

float FCoreTweenEasing::OutElastic(const float Alpha, const float Amplitude)
{
	if (Alpha == 0.0f || Alpha == 1.0f)
	{
		return Alpha;
	}
	const float Period = 0.3f;
	float S;
	float A = Amplitude;
	if (A < 1.0f)
	{
		A = 1.0f;
		S = Period / 4.0f;
	}
	else
	{
		S = Period / (2.0f * PI) * FMath::Asin(1.0f / A);
	}
	return A * FMath::Pow(2.0f, -10.0f * Alpha) * FMath::Sin((Alpha - S) * (2.0f * PI) / Period) + 1.0f;
}

float FCoreTweenEasing::InOutElastic(const float Alpha, const float Amplitude)
{
	if (Alpha == 0.0f || Alpha == 1.0f)
	{
		return Alpha;
	}
	const float Period = 0.3f * 1.5f;
	float S;
	float A = Amplitude;
	if (A < 1.0f)
	{
		A = 1.0f;
		S = Period / 4.0f;
	}
	else
	{
		S = Period / (2.0f * PI) * FMath::Asin(1.0f / A);
	}
	const float T = Alpha * 2.0f;
	if (T < 1.0f)
	{
		const float Shifted = T - 1.0f;
		return -0.5f * (A * FMath::Pow(2.0f, 10.0f * Shifted) * FMath::Sin((Shifted - S) * (2.0f * PI) / Period));
	}
	const float Shifted = T - 1.0f;
	return A * FMath::Pow(2.0f, -10.0f * Shifted) * FMath::Sin((Shifted - S) * (2.0f * PI) / Period) * 0.5f + 1.0f;
}

// ── Bounce ──────────────────────────────────────────────────────────

float FCoreTweenEasing::OutBounce(float Alpha)
{
	if (Alpha < (1.0f / 2.75f))
	{
		return 7.5625f * Alpha * Alpha;
	}
	if (Alpha < (2.0f / 2.75f))
	{
		Alpha -= 1.5f / 2.75f;
		return 7.5625f * Alpha * Alpha + 0.75f;
	}
	if (Alpha < (2.5f / 2.75f))
	{
		Alpha -= 2.25f / 2.75f;
		return 7.5625f * Alpha * Alpha + 0.9375f;
	}
	Alpha -= 2.625f / 2.75f;
	return 7.5625f * Alpha * Alpha + 0.984375f;
}

float FCoreTweenEasing::InBounce(const float Alpha)
{
	return 1.0f - OutBounce(1.0f - Alpha);
}

float FCoreTweenEasing::InOutBounce(const float Alpha)
{
	if (Alpha < 0.5f)
	{
		return InBounce(Alpha * 2.0f) * 0.5f;
	}
	return OutBounce(Alpha * 2.0f - 1.0f) * 0.5f + 0.5f;
}

// ── Spring ──────────────────────────────────────────────────────────

float FCoreTweenEasing::SpringEase(const float Alpha, const float Damping)
{
	// Critically damped spring approximation
	const float ClampedDamping = FMath::Clamp(Damping, 0.0f, 1.0f);
	const float Frequency = 10.0f;
	const float Decay = FMath::Exp(-ClampedDamping * Frequency * Alpha);
	return 1.0f - Decay * FMath::Cos(Frequency * (1.0f - ClampedDamping) * Alpha);
}


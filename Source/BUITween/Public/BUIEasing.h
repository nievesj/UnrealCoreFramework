#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"

UENUM(BlueprintType)
enum class EBUIEasingType : uint8
{
	Linear,
	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,
	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,
	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,
	EaseInSine,
	EaseOutSine,
	EaseInOutSine,
	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce
};

struct FBUIEasing
{
public:
#define BUI_TWO_PI (6.28318530717f)

	static float Ease(EBUIEasingType Type, float time, float duration = 1.0f)
	{
		switch (Type)
		{
			case EBUIEasingType::Linear:
				return Linear(time, duration);
			case EBUIEasingType::EaseInQuad:
				return InQuad(time, duration);
			case EBUIEasingType::EaseOutQuad:
				return OutQuad(time, duration);
			case EBUIEasingType::EaseInOutQuad:
				return InOutQuad(time, duration);
			case EBUIEasingType::EaseInCubic:
				return InCubic(time, duration);
			case EBUIEasingType::EaseOutCubic:
				return OutCubic(time, duration);
			case EBUIEasingType::EaseInOutCubic:
				return InOutCubic(time, duration);
			case EBUIEasingType::EaseInExpo:
				return InExpo(time, duration);
			case EBUIEasingType::EaseOutExpo:
				return OutExpo(time, duration);
			case EBUIEasingType::EaseInOutExpo:
				return InOutExpo(time, duration);
			case EBUIEasingType::EaseInSine:
				return InSine(time, duration);
			case EBUIEasingType::EaseOutSine:
				return OutSine(time, duration);
			case EBUIEasingType::EaseInOutSine:
				return InOutSine(time, duration);
			case EBUIEasingType::EaseInBounce:
				return InBounce(time, duration);
			case EBUIEasingType::EaseOutBounce:
				return OutBounce(time, duration);
			case EBUIEasingType::EaseInOutBounce:
				return InOutBounce(time, duration);
		}
		return 0;
	}

	static float Linear(float time, float duration = 1.0f)
	{
		return time / duration;
	}

	static float InQuad(float time, float duration = 1.0f)
	{
		time /= duration;
		return time * time;
	}

	static float OutQuad(float time, float duration = 1.0f)
	{
		time /= duration;
		return -time * (time - 2);
	}

	static float InOutQuad(float time, float duration = 1.0f)
	{
		time /= duration * 0.5f;
		if (time < 1)
			return 0.5f * time * time;
		--time;
		return -0.5f * (time * (time - 2) - 1);
	}

	static float InCubic(float time, float duration = 1.0f)
	{
		time /= duration;
		return time * time * time;
	}

	static float OutCubic(float time, float duration = 1.0f)
	{
		time = time / duration - 1;
		return (time * time * time + 1);
	}

	static float InOutCubic(float time, float duration = 1.0f)
	{
		time /= duration * 0.5f;
		if (time < 1)
			return 0.5f * time * time * time;
		time -= 2;
		return 0.5f * (time * time * time + 2);
	}

	static float InExpo(float time, float duration = 1.0f)
	{
		return (time == 0) ? 0 : (float)FMath::Pow(2, 10 * (time / duration - 1));
	}

	static float OutExpo(float time, float duration = 1.0f)
	{
		if (time == duration)
			return 1;
		return (-(float)FMath::Pow(2, -10 * time / duration) + 1);
	}

	static float InOutExpo(float time, float duration = 1.0f)
	{
		if (time == 0)
			return 0;
		if (time == duration)
			return 1;
		time /= duration * 0.5f;
		if (time < 1)
			return 0.5f * (float)FMath::Pow(2, 10 * (time - 1));
		--time;
		return 0.5f * (-(float)FMath::Pow(2, -10 * time) + 2);
	}

	static float InSine(float time, float duration = 1.0f)
	{
		return -(float)FMath::Cos(time / duration * HALF_PI) + 1;
	}

	static float OutSine(float time, float duration = 1.0f)
	{
		return (float)FMath::Sin(time / duration * HALF_PI);
	}

	static float InOutSine(float time, float duration = 1.0f)
	{
		return -0.5f * ((float)FMath::Cos(PI * time / duration) - 1);
	}

	// --- Bounce Easing Functions ---
	static float OutBounce(float time, float duration = 1.0f)
	{
		time /= duration;
		if (time < (1 / 2.75f))
		{
			return 7.5625f * time * time;
		}
		else if (time < (2 / 2.75f))
		{
			time -= (1.5f / 2.75f);
			return 7.5625f * time * time + 0.75f;
		}
		else if (time < (2.5f / 2.75f))
		{
			time -= (2.25f / 2.75f);
			return 7.5625f * time * time + 0.9375f;
		}
		else
		{
			time -= (2.625f / 2.75f);
			return 7.5625f * time * time + 0.984375f;
		}
	}

	static float InBounce(float time, float duration = 1.0f)
	{
		return 1.0f - OutBounce(duration - time, duration);
	}

	static float InOutBounce(float time, float duration = 1.0f)
	{
		if (time < duration * 0.5f)
			return InBounce(time * 2, duration) * 0.5f;
		else
			return OutBounce(time * 2 - duration, duration) * 0.5f + 0.5f;
	}
};

#undef BUI_TWO_PI

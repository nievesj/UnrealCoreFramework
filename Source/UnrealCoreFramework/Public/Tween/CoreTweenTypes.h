#pragma once

#include "CoreTweenTypes.generated.h"

DECLARE_DYNAMIC_DELEGATE(FTweenCompleteDelegate);

UENUM(BlueprintType)
enum class ETweenEaseType : uint8
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
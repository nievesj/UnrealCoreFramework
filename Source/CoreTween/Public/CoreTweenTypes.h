#pragma once

#include "UObject/ObjectMacros.h"

#include "CoreTweenTypes.generated.h"

DECLARE_DELEGATE(FCoreTweenSignature);
DECLARE_DYNAMIC_DELEGATE(FCoreTweenBPSignature);

UENUM(BlueprintType)
enum class ECoreTweenTimeSource : uint8
{
	/** Game-dilated time. Pauses with the game. */
	GameTime,
	/** Unpaused time. Ticks during pause. Default for widget tweens. */
	Unpaused,
	/** Wall-clock real time. Ignores pause and time dilation. */
	RealTime
};


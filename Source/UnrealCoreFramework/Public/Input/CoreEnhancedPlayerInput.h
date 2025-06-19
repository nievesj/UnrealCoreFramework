// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#pragma once

#include "EnhancedPlayerInput.h"

#include "CoreEnhancedPlayerInput.generated.h"

/**
 *
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UCoreEnhancedPlayerInput : public UEnhancedPlayerInput
{
	GENERATED_BODY()

public:
	virtual void ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused) override;
};

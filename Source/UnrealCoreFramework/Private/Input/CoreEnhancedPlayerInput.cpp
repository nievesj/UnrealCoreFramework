// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#include "Input/CoreEnhancedPlayerInput.h"

#include "Engine/GameInstance.h"

void UCoreEnhancedPlayerInput::ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused)
{
	Super::ProcessInputStack(InputComponentStack, DeltaTime, bGamePaused);
}
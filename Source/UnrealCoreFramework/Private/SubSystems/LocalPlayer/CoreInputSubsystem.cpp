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

#include "SubSystems/LocalPlayer/CoreInputSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/InputDeviceSubsystem.h"

void UCoreInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing Input Subsystem"))

	InputDeviceSubsystem = GEngine->GetEngineSubsystem<UInputDeviceSubsystem>();
	if (InputDeviceSubsystem)
	{
		InputDeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &UCoreInputSubsystem::HandleOnHardwareInputDeviceChanged);
	}
}

void UCoreInputSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitializing Input Subsystem"))
	if (InputDeviceSubsystem)
	{
		InputDeviceSubsystem->OnInputHardwareDeviceChanged.RemoveAll(this);
	}
}

void UCoreInputSubsystem::HandleOnHardwareInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer<ULocalPlayer>())
	{
		if (LocalPlayer->GetPlatformUserId() == UserId)
		{
			// XInputInterface
			// XInputController
			// Dualsense
			const FHardwareDeviceIdentifier& HardwareDeviceIdentifier = InputDeviceSubsystem->GetMostRecentlyUsedHardwareDevice(UserId);
			const ECoreInputDeviceType CoreInputDeviceType = DetermineControllerType(HardwareDeviceIdentifier);
			if (OnCoreInputDeviceChanged.IsBound())
			{
				OnCoreInputDeviceChanged.Broadcast(CoreInputDeviceType);
			}

			UE_LOG(LogTemp, Log, TEXT("DeviceId %d | UserId %d | PrimaryDeviceType %s"), DeviceId.GetId(), UserId.GetInternalId(), *UEnum::GetValueAsString(HardwareDeviceIdentifier.PrimaryDeviceType));
		}

		// UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
		//   EnhancedInputComponent->RemoveBindingByHandle(m_GoBackActionBindingHandle);
	}
}

ECoreInputDeviceType UCoreInputSubsystem::DetermineControllerType(const FHardwareDeviceIdentifier& HardwareDeviceIdentifier)
{
	switch (HardwareDeviceIdentifier.PrimaryDeviceType)
	{
		case EHardwareDevicePrimaryType::KeyboardAndMouse:
			return ECoreInputDeviceType::KeyboardMouse;
		case EHardwareDevicePrimaryType::Gamepad:
			return ECoreInputDeviceType::Gamepad;
		default:
			return ECoreInputDeviceType::KeyboardMouse;
	}
}

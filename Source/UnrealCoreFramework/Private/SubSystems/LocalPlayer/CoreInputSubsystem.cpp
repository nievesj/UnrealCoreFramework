// Fill out your copyright notice in the Description page of Project Settings.

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
			const ECoreInputDeviceType		 CoreInputDeviceType = DetermineControllerType(HardwareDeviceIdentifier);
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

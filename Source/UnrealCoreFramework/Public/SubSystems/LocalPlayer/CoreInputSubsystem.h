// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Input/InputCoreFrameworkTypes.h"
#include "SubSystems/Base/CoreLocalPlayerSubsystem.h"

#include "CoreInputSubsystem.generated.h"

struct FHardwareDeviceIdentifier;
class UInputDeviceSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoreInputDeviceChanged, const ECoreInputDeviceType, CoreInputDeviceType);

UCLASS()
class UNREALCOREFRAMEWORK_API UCoreInputSubsystem : public UCoreLocalPlayerSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION()
	void HandleOnHardwareInputDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);

	ECoreInputDeviceType DetermineControllerType(const FHardwareDeviceIdentifier& HardwareDeviceIdentifier);

	UPROPERTY(BlueprintAssignable)
	FCoreInputDeviceChanged OnCoreInputDeviceChanged;

	UPROPERTY(Transient)
	UInputDeviceSubsystem* InputDeviceSubsystem;
};

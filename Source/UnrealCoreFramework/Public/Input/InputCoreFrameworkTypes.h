// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#pragma once

#include "Data/DataCoreFrameworkTypes.h"

#include "InputCoreFrameworkTypes.generated.h"

class UInputAction;
class UTexture2D;

UENUM(BlueprintType)
enum class ECoreInputDeviceType : uint8
{
	None,
	KeyboardMouse,
	Gamepad,
	XboxGamePad,
	PS5GamePad
};

USTRUCT(BlueprintType)
struct FInputImageDefinitionRow : public FCoreTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UInputAction> InputAction;
};

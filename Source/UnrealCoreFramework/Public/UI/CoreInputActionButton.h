// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#pragma once

#include "CommonButtonBase.h"

#include "CoreInputActionButton.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, Blueprintable, ClassGroup = UI, meta = (Category = "Core Framework UI", DisableNativeTick))
class UNREALCOREFRAMEWORK_API UCoreInputActionButton : public UCommonButtonBase
{
	GENERATED_BODY()

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnCurrentTextStyleChanged() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CoreUI)
	FText ButtonLabelText = FText::FromString("Button Label");

	UPROPERTY(BlueprintReadOnly, Category = CoreUI, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UCommonTextBlock> InputActionLabel;
};

#pragma once

#include "CoreWidget.h"
#include "GameFramework/HUD.h"

#include "CoreHUD.generated.h"

/**
 *
 */
UCLASS()
class UNREALCOREFRAMEWORK_API ACoreHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = UnrealCoreFrameworkSettings)
	virtual void ShowMainHUD();

	UFUNCTION(BlueprintCallable, Category = UnrealCoreFrameworkSettings)
	virtual void HideMainHUD();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnrealCoreFrameworkSettings, Meta = (DisplayName = "Main HUD Class"))
	TSubclassOf<UCoreWidget> MainPageClass;

	UPROPERTY(Transient)
	UCoreWidget* MainPage;
};
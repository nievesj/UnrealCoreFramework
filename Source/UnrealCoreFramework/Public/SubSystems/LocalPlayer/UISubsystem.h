// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SubSystems/Base/CoreLocalPlayerSubsystem.h"
#include "UI/PageableWidgetInterface.h"

#include "UISubsystem.generated.h"

enum class EWidgetContainerType : uint8;
class UMainUiContainer;
class UCorePage;
class ACorePlayerController;
class UCoreWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogUISubsystem, Log, All);

UENUM(BlueprintType)
enum class ECoreMainPageType : uint8
{
	MainMenu,
	MainHUD,
	PauseMenu
};

UCLASS()
class UNREALCOREFRAMEWORK_API UUISubsystem : public UCoreLocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UCoreWidget* CreateViewportPage(APlayerController* Owner, TSubclassOf<UCoreWidget> PageClass);
	void		 RemoveViewportPage(IPageableWidgetInterface* Page);
	void		 RemoveAllViewportPages();

	UCoreWidget* AddWidgetToStack(const TSubclassOf<UCoreWidget>& PageClass, const EWidgetContainerType& StackContainerType);
	void		 RemoveWidgetFromStack(UCoreWidget& Widget, const EWidgetContainerType& StackContainerType, bool Destroy = false);

	/** Navigate back to the previous page in history. */
	void GoBack();


	/** Creates the main UI container from settings. */
	void CreateMainUIContainer();

	/** Returns the top-most page in the open widget stack. */
	IPageableWidgetInterface* GetTopPage();

	UMainUiContainer* GetMainUIContainer() const
	{
		return MainUiContainer;
	};

	bool ShouldDisablePlayerControllerInput();
	void SetPlayerControllerInput(APlayerController* PC, bool bIsDisabled);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	UPROPERTY(Transient)
	TArray<UCoreWidget*> CoreWidgetsOpen;

	UPROPERTY(Transient)
	TArray<TSubclassOf<UCorePage>> PageHistory;

	UPROPERTY(Transient)
	UMainUiContainer* MainUiContainer;
};

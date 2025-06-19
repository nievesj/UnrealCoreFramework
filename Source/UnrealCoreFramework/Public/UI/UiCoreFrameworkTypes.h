// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Object.h"

#include "UiCoreFrameworkTypes.generated.h"

enum class ETweenEaseType : uint8;
enum class ETransitionCurve : uint8;
enum class ECommonSwitcherTransition : uint8;
enum class EBUIEasingType : uint8;

namespace EUMGSequencePlayMode
{
	enum Type : int;
}

UENUM(BlueprintType)
enum class EWidgetContainerType : uint8
{
	HUD,
	Page,
	Modal,
};

UENUM(BlueprintType)
enum class EWidgetTransitionType : uint8
{
	NotUsed,
	Scale,
	Translation,
	Fade
};

UENUM(BlueprintType)
enum class EWidgetTranslationType : uint8
{
	None,
	FromLeft,
	FromRight,
	FromTop,
	FromBottom,
};

UENUM(BlueprintType)
enum class EWidgetTransitionMode : uint8
{
	Intro,
	Outtro,
};

UENUM(BlueprintType)
enum class EWidgetAnimationType : uint8
{
	None,
	CommonUiDefault,
	WidgetTween,
	WidgetAnimation,
};

USTRUCT(BlueprintType)
struct FCommonUiTransitionOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	ECommonSwitcherTransition TransitionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	ETransitionCurve TransitionCurveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Transition)
	float TransitionDuration = 0.4f;
};

USTRUCT(BlueprintType)
struct FWidgetTweenTransitionOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	EWidgetTransitionType TransitionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	EWidgetTranslationType WidgetTranslationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	EBUIEasingType EasingType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	float TransitionTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	float FadeFrom = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	float FadeTo = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "TransitionType == EWidgetTransitionType::Scale"))
	FVector2D ScaleFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "TransitionType == EWidgetTransitionType::Scale"))
	FVector2D ScaleTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "TransitionType == EWidgetTransitionType::Translation"))
	bool UseViewportAsTranslationOrigin = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "UseViewportAsTranslationOrigin == false && TransitionType == EWidgetTransitionType::Translation"))
	FVector2D TranslationFrom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "UseViewportAsTranslationOrigin == false && TransitionType == EWidgetTransitionType::Translation"))
	FVector2D TranslationTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "TransitionType == EWidgetTransitionType::Translation"))
	FVector2D TranslationFromOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions, meta = (EditCondition = "TransitionType == EWidgetTransitionType::Translation"))
	FVector2D TranslationToOffset;
};

USTRUCT(BlueprintType)
struct FWidgetAnimationOptions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	float StartAtTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	int32 NumberOfLoops = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	TEnumAsByte<EUMGSequencePlayMode::Type> PlayMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	float PlaybackSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WidgetTransitionOptions)
	bool bRestoreState = false;
};

USTRUCT(BlueprintType)
struct FCoreWidgetAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget)
	EWidgetAnimationType WidgetAnimationType = EWidgetAnimationType::WidgetTween;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (EditCondition = "WidgetAnimationType == EWidgetAnimationType::CommonUiDefault"))
	FCommonUiTransitionOptions CommonUiTransitionOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetTween"))
	FWidgetTweenTransitionOptions TweenEntranceOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetTween"))
	FWidgetTweenTransitionOptions TweenExitOptions;

	// UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (BindWidgetAnimOptional, EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetAnimation"))
	// UWidgetAnimation* WidgetAnimationIntro;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (BindWidgetAnimOptional, EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetAnimation"))
	// FWidgetAnimationOptions WidgetAnimationOptionsIntro;

	// UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (BindWidgetAnimOptional, EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetAnimation"))
	// UWidgetAnimation* WidgetAnimationOuttro;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CoreWidget, meta = (BindWidgetAnimOptional, EditCondition = "WidgetAnimationType == EWidgetAnimationType::WidgetAnimation"))
	// FWidgetAnimationOptions WidgetAnimationOptionsOuttro;
};
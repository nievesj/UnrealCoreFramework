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

#pragma once

#include "UObject/Object.h"

#include "UiCoreFrameworkTypes.generated.h"

enum class ETransitionCurve : uint8;
enum class ECommonSwitcherTransition : uint8;
enum class ECoreTweenEasingType : uint8;

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
enum class ECoreModalResult : uint8
{
	/** User pressed the confirm button (OK / Yes) */
	Confirmed,
	/** User pressed the cancel button (Cancel / No) */
	Cancelled,
	/** Modal was dismissed without an explicit choice (close button, escape, etc.) */
	Dismissed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModalResultDelegate, ECoreModalResult, Result);

UENUM(BlueprintType)
enum class EWidgetAnimationType : uint8
{
	None,
	CommonUiDefault,
	WidgetTween,
	WidgetAnimation,
};

UENUM(BlueprintType)
enum class EWidgetTransitionMode : uint8
{
	Intro,
	Outro,
};

UENUM(BlueprintType)
enum class EWidgetTransitionType : uint8
{
	Fade,
	Scale,
	Translation,
};

UENUM(BlueprintType)
enum class EWidgetTranslationType : uint8
{
	FromLeft,
	FromRight,
	FromTop,
	FromBottom,
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
	ECoreTweenEasingType EasingType;

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
};
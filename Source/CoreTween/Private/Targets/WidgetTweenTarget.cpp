#include "Targets/WidgetTweenTarget.h"

#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Widget.h"

FWidgetTweenTarget::FWidgetTweenTarget(UWidget* InWidget)
	: WeakWidget(InWidget)
{
}

bool FWidgetTweenTarget::IsTargetValid() const
{
	return WeakWidget.IsValid();
}

UWidget* FWidgetTweenTarget::GetWidget() const
{
	return WeakWidget.Get();
}

// ── Read current values ─────────────────────────────────────────────

FVector2D FWidgetTweenTarget::GetCurrentTranslation() const
{
	UWidget* W = WeakWidget.Get();
	return W ? W->GetRenderTransform().Translation : FVector2D::ZeroVector;
}

FVector2D FWidgetTweenTarget::GetCurrentScale() const
{
	UWidget* W = WeakWidget.Get();
	return W ? W->GetRenderTransform().Scale : FVector2D::UnitVector;
}

float FWidgetTweenTarget::GetCurrentOpacity() const
{
	UWidget* W = WeakWidget.Get();
	return W ? W->GetRenderOpacity() : 1.0f;
}

FLinearColor FWidgetTweenTarget::GetCurrentColor() const
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return FLinearColor::White;
	}
	if (UUserWidget* UW = Cast<UUserWidget>(W))
	{
		return UW->GetColorAndOpacity();
	}
	if (UImage* UI = Cast<UImage>(W))
	{
		return UI->GetColorAndOpacity();
	}
	if (UBorder* Border = Cast<UBorder>(W))
	{
		return Border->GetContentColorAndOpacity();
	}
	return FLinearColor::White;
}

float FWidgetTweenTarget::GetCurrentRotation() const
{
	UWidget* W = WeakWidget.Get();
	return W ? W->GetRenderTransform().Angle : 0.0f;
}

FVector2D FWidgetTweenTarget::GetCurrentCanvasPosition() const
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return FVector2D::ZeroVector;
	}
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(W->Slot);
	return CanvasSlot ? CanvasSlot->GetPosition() : FVector2D::ZeroVector;
}

FVector4 FWidgetTweenTarget::GetCurrentPadding() const
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return FVector4(0, 0, 0, 0);
	}
	// Consistent LTRB order: Left, Top, Right, Bottom
	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(W->Slot))
	{
		const FMargin& P = OverlaySlot->GetPadding();
		return FVector4(P.Left, P.Top, P.Right, P.Bottom);
	}
	if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(W->Slot))
	{
		const FMargin& P = HBoxSlot->GetPadding();
		return FVector4(P.Left, P.Top, P.Right, P.Bottom);
	}
	if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(W->Slot))
	{
		const FMargin& P = VBoxSlot->GetPadding();
		return FVector4(P.Left, P.Top, P.Right, P.Bottom);
	}
	return FVector4(0, 0, 0, 0);
}

ESlateVisibility FWidgetTweenTarget::GetCurrentVisibility() const
{
	UWidget* W = WeakWidget.Get();
	return W ? W->GetVisibility() : ESlateVisibility::Collapsed;
}

float FWidgetTweenTarget::GetCurrentMaxDesiredHeight() const
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return 0.0f;
	}
	USizeBox* SizeBox = Cast<USizeBox>(W);
	return SizeBox ? SizeBox->GetMaxDesiredHeight() : 0.0f;
}

// ── Apply interpolated values ───────────────────────────────────────

void FWidgetTweenTarget::ApplyTranslation(const FVector2D& Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TweenTarget] ApplyTranslation — widget is null"));
		return;
	}
	FWidgetTransform Transform = W->GetRenderTransform();
	Transform.Translation = Value;
	W->SetRenderTransform(Transform);
	UE_LOG(LogTemp, Verbose, TEXT("[TweenTarget] ApplyTranslation(%f, %f) on %s"), Value.X, Value.Y, *W->GetName());
}

void FWidgetTweenTarget::ApplyScale(const FVector2D& Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TweenTarget] ApplyScale — widget is null"));
		return;
	}
	FWidgetTransform Transform = W->GetRenderTransform();
	Transform.Scale = Value;
	W->SetRenderTransform(Transform);
	UE_LOG(LogTemp, Verbose, TEXT("[TweenTarget] ApplyScale(%f, %f) on %s"), Value.X, Value.Y, *W->GetName());
}

void FWidgetTweenTarget::ApplyOpacity(const float Value)
{
	UWidget* W = WeakWidget.Get();
	if (W)
	{
		W->SetRenderOpacity(Value);
		UE_LOG(LogTemp, Verbose, TEXT("[TweenTarget] ApplyOpacity(%f) on %s"), Value, *W->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TweenTarget] ApplyOpacity — widget is null"));
	}
}

void FWidgetTweenTarget::ApplyColor(const FLinearColor& Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TweenTarget] ApplyColor — widget is null"));
		return;
	}
	if (UUserWidget* UW = Cast<UUserWidget>(W))
	{
		UW->SetColorAndOpacity(Value);
	}
	if (UImage* UI = Cast<UImage>(W))
	{
		UI->SetColorAndOpacity(Value);
	}
	if (UBorder* Border = Cast<UBorder>(W))
	{
		Border->SetContentColorAndOpacity(Value);
	}
}

void FWidgetTweenTarget::ApplyRotation(const float Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return;
	}
	FWidgetTransform Transform = W->GetRenderTransform();
	Transform.Angle = Value;
	W->SetRenderTransform(Transform);
}

void FWidgetTweenTarget::ApplyCanvasPosition(const FVector2D& Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return;
	}
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(W->Slot);
	if (CanvasSlot)
	{
		CanvasSlot->SetPosition(Value);
	}
}

void FWidgetTweenTarget::ApplyPadding(const FVector4& Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return;
	}
	// LTRB: Value.X=Left, Value.Y=Top, Value.Z=Right, Value.W=Bottom
	const FMargin Margin(Value.X, Value.Y, Value.Z, Value.W);
	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(W->Slot))
	{
		OverlaySlot->SetPadding(Margin);
	}
	else if (UHorizontalBoxSlot* HBoxSlot = Cast<UHorizontalBoxSlot>(W->Slot))
	{
		HBoxSlot->SetPadding(Margin);
	}
	else if (UVerticalBoxSlot* VBoxSlot = Cast<UVerticalBoxSlot>(W->Slot))
	{
		VBoxSlot->SetPadding(Margin);
	}
}

void FWidgetTweenTarget::ApplyVisibility(const ESlateVisibility Value)
{
	UWidget* W = WeakWidget.Get();
	if (W)
	{
		W->SetVisibility(Value);
	}
}

void FWidgetTweenTarget::ApplyMaxDesiredHeight(const float Value)
{
	UWidget* W = WeakWidget.Get();
	if (!W)
	{
		return;
	}
	USizeBox* SizeBox = Cast<USizeBox>(W);
	if (SizeBox)
	{
		SizeBox->SetMaxDesiredHeight(Value);
	}
}


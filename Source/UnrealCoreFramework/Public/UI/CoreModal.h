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

#include "CoreWidget.h"
#include "UiCoreFrameworkTypes.h"
#include "AsyncFlowTask.h"

#include "CoreModal.generated.h"

class UButton;
class UTextBlock;
class UUISubsystem;

/**
 * Base class for modal dialogs. Displays a centered overlay that blocks
 * input to the widgets behind it. Supports confirm/cancel/dismiss results.
 *
 * C++ callers can co_await ShowAndWaitTask() to get the result inline:
 *
 *     ECoreModalResult Result = co_await Modal->ShowAndWaitTask();
 *     if (Result == ECoreModalResult::Confirmed) { ... }
 *
 * Blueprint callers bind to OnModalResult for the same information.
 */
UCLASS(Abstract, Blueprintable, ClassGroup = UI, meta = (Category = "Core Framework UI", DisableNativeTick))
class UNREALCOREFRAMEWORK_API UCoreModal : public UCoreWidget
{
	GENERATED_BODY()

public:
	/**
	 * Show the modal and co_await until the user makes a choice.
	 * @return The user's response (Confirmed, Cancelled, or Dismissed).
	 */
	AsyncFlow::TTask<ECoreModalResult> ShowAndWaitTask();

	/** Resolve the modal with Confirmed. */
	UFUNCTION(BlueprintCallable, Category = CoreModal)
	void Confirm();

	/** Resolve the modal with Cancelled. */
	UFUNCTION(BlueprintCallable, Category = CoreModal)
	void Cancel();

	/** Resolve the modal with Dismissed (close without explicit choice). */
	UFUNCTION(BlueprintCallable, Category = CoreModal)
	void Dismiss();

	/** Set the title text (only applies if TitleText widget is bound). */
	UFUNCTION(BlueprintCallable, Category = CoreModal)
	void SetTitle(const FText& InTitle);

	/** Set the message body text (only applies if MessageText widget is bound). */
	UFUNCTION(BlueprintCallable, Category = CoreModal)
	void SetMessage(const FText& InMessage);

	/** Fired when the modal resolves with any result. */
	UPROPERTY(BlueprintAssignable, Category = "CoreModal|Events")
	FOnModalResultDelegate OnModalResult;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void InternalShown() override;
	virtual void InternalHidden() override;

	/** Called in Blueprint when the modal resolves (before it hides). */
	UFUNCTION(BlueprintImplementableEvent, Category = CoreModal)
	void OnModalResolved(ECoreModalResult Result);

	// ── Optional bound widgets ──────────────────────────────────────

	UPROPERTY(BlueprintReadWrite, Category = CoreModal, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(BlueprintReadWrite, Category = CoreModal, meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

	UPROPERTY(BlueprintReadWrite, Category = CoreModal, meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY(BlueprintReadWrite, Category = CoreModal, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(BlueprintReadWrite, Category = CoreModal, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

private:
	UFUNCTION()
	void Handle_OnConfirmClicked();

	UFUNCTION()
	void Handle_OnCancelClicked();

	UFUNCTION()
	void Handle_OnCloseClicked();

	/** Finalize with the given result: broadcast, hide, and resolve the awaitable. */
	void ResolveWithResult(ECoreModalResult Result);

	void BindButtons();
	void UnbindButtons();

	/** Result stashed between Resolve and the hide-animation completion. */
	ECoreModalResult PendingResult = ECoreModalResult::Dismissed;

	/** Whether we're waiting for the hide animation to finish after resolving. */
	bool bIsResolving = false;

	/** True when ShowAndWaitTask() is active — the coroutine handles hiding. */
	bool bAwaitableMode = false;
};



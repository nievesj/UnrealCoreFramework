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

#include "UI/CoreModal.h"

#include "Async/CoreAsyncTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AsyncFlow.h"
#include "AsyncFlowAwaiters.h"

void UCoreModal::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoreModal::NativeDestruct()
{
	UnbindButtons();
	Super::NativeDestruct();
}

void UCoreModal::InternalShown()
{
	BindButtons();
	Super::InternalShown();
}

void UCoreModal::InternalHidden()
{
	UnbindButtons();
	bIsResolving = false;
	bAwaitableMode = false;
	Super::InternalHidden();
}

// ── Public API ──────────────────────────────────────────────────────

AsyncFlow::TTask<ECoreModalResult> UCoreModal::ShowAndWaitTask()
{
	UCF_ASYNC_CONTRACT(this);

	bAwaitableMode = true;
	bIsResolving = false;
	PendingResult = ECoreModalResult::Dismissed;

	Show();

	while (!bIsResolving)
	{
		co_await AsyncFlow::NextTick(this);
	}

	co_await HideTask();

	co_return PendingResult;
}

void UCoreModal::Confirm()
{
	ResolveWithResult(ECoreModalResult::Confirmed);
}

void UCoreModal::Cancel()
{
	ResolveWithResult(ECoreModalResult::Cancelled);
}

void UCoreModal::Dismiss()
{
	ResolveWithResult(ECoreModalResult::Dismissed);
}

void UCoreModal::SetTitle(const FText& InTitle)
{
	if (TitleText)
	{
		TitleText->SetText(InTitle);
	}
}

void UCoreModal::SetMessage(const FText& InMessage)
{
	if (MessageText)
	{
		MessageText->SetText(InMessage);
	}
}

// ── Resolution ──────────────────────────────────────────────────────

void UCoreModal::ResolveWithResult(const ECoreModalResult Result)
{
	if (bIsResolving)
	{
		return;
	}

	PendingResult = Result;
	bIsResolving = true;

	OnModalResolved(Result);
	OnModalResult.Broadcast(Result);

	// In fire-and-forget mode, trigger hide directly.
	// In awaitable mode, the coroutine loop in ShowAndWaitTask() handles hiding.
	if (!bAwaitableMode)
	{
		Hide();
	}
}

// ── Button binding ──────────────────────────────────────────────────

void UCoreModal::BindButtons()
{
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UCoreModal::Handle_OnConfirmClicked);
	}
	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UCoreModal::Handle_OnCancelClicked);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UCoreModal::Handle_OnCloseClicked);
	}
}

void UCoreModal::UnbindButtons()
{
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.RemoveAll(this);
	}
	if (CancelButton)
	{
		CancelButton->OnClicked.RemoveAll(this);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.RemoveAll(this);
	}
}

void UCoreModal::Handle_OnConfirmClicked()
{
	Confirm();
}

void UCoreModal::Handle_OnCancelClicked()
{
	Cancel();
}

void UCoreModal::Handle_OnCloseClicked()
{
	Dismiss();
}


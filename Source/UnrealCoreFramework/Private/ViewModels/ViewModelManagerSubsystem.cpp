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

#include "ViewModels/ViewModelManagerSubsystem.h"

#include "ViewModels/CoreViewModel.h"

DEFINE_LOG_CATEGORY(LogViewModelManagerSubsystem);

UCoreViewModel* UViewModelManagerSubsystem::GetOrCreateViewModel(const TSubclassOf<UCoreViewModel> ViewModelClass, UObject* TrackedObject)
{
	if (!ViewModelClass)
	{ // problem here, null check fails even there is somethin in ViewModelClass
		UE_LOG(LogViewModelManagerSubsystem, Warning, TEXT("CreateViewModel: Invalid ViewModel class"));
		return nullptr;
	}

	if (!IsValid(TrackedObject))
	{
		UE_LOG(LogViewModelManagerSubsystem, Warning, TEXT("CreateViewModel: Invalid TrackedObject"));
		return nullptr;
	}

	UCoreViewModel* ViewModel = GetModel(TrackedObject);
	if (IsValid(ViewModel))
	{
		return ViewModel;
	}

	// Create new ViewModel instance
	UCoreViewModel* NewViewModel = NewObject<UCoreViewModel>(this, ViewModelClass);
	if (IsValid(NewViewModel))
	{
		NewViewModel->SetSource(TrackedObject);
		ViewModels.Add(TrackedObject->GetUniqueID(), NewViewModel);
		UE_LOG(LogViewModelManagerSubsystem, Log, TEXT("Created ViewModel of class: %s"), *ViewModelClass->GetName());
		return NewViewModel;
	}

	UE_LOG(LogViewModelManagerSubsystem, Error, TEXT("Failed to create ViewModel of class: %s"), *ViewModelClass->GetName());
	return nullptr;
}

UCoreViewModel* UViewModelManagerSubsystem::GetModel(const UObject* TrackedObject)
{
	if (!IsValid(TrackedObject))
	{
		return nullptr;
	}

	const TObjectPtr<UCoreViewModel>* Model = ViewModels.Find(TrackedObject->GetUniqueID());
	if (!Model || !IsValid(*Model))
	{
		return nullptr;
	}

	UCoreViewModel* ViewModel = Model->Get();
	const UObject* Source = ViewModel->GetSource();
	// Verify the source matches and is still valid
	if (!IsValid(Source) || Source->GetUniqueID() != TrackedObject->GetUniqueID())
	{
		// Clean up stale entry
		ViewModel->DeInitialize();
		ViewModels.Remove(TrackedObject->GetUniqueID());
		return nullptr;
	}

	return ViewModel;
}

bool UViewModelManagerSubsystem::RemoveViewModel(const UObject* TrackedObject)
{
	if (!IsValid(TrackedObject))
	{
		return false;
	}

	UCoreViewModel* Model = GetModel(TrackedObject);
	if (IsValid(Model))
	{
		Model->DeInitialize();
		ViewModels.Remove(TrackedObject->GetUniqueID());
		Model = nullptr;
		return true;
	}

	return false;
}

void UViewModelManagerSubsystem::ClearModels()
{
	for (const auto& Pair : ViewModels)
	{
		if (UCoreViewModel* ViewModel = Pair.Value.Get())
		{
			if (IsValid(ViewModel))
			{
				ViewModel->DeInitialize();
			}
		}
	}

	ViewModels.Empty();
}

// Fill out your copyright notice in the Description page of Project Settings.

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
		ViewModels.Add(TrackedObject->GetUniqueID());
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
	const UObject*	Source = ViewModel->GetSource();
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

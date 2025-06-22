// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/ViewModelManagerSubsystem.h"

#include "ViewModels/CoreViewModel.h"

DEFINE_LOG_CATEGORY(LogViewModelManagerSubsystem);

UCoreViewModel* UViewModelManagerSubsystem::CreateViewModel(const TSubclassOf<UCoreViewModel> ViewModelClass)
{
	if (!IsValid(ViewModelClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateViewModel: Invalid ViewModel class"));
		return nullptr;
	}

	// Check if we already have this ViewModel
	if (TObjectPtr<UCoreViewModel>* ExistingViewModel = ViewModels.Find(ViewModelClass))
	{
		if (IsValid(*ExistingViewModel))
		{
			return *ExistingViewModel;
		}
		else
		{
			// Remove invalid entry
			ViewModels.Remove(ViewModelClass);
		}
	}

	// Create new ViewModel instance
	UCoreViewModel* NewViewModel = NewObject<UCoreViewModel>(this, ViewModelClass);
	if (IsValid(NewViewModel))
	{
		ViewModels.Add(ViewModelClass, NewViewModel);
		UE_LOG(LogTemp, Log, TEXT("Created ViewModel of class: %s"), *ViewModelClass->GetName());
		return NewViewModel;
	}

	UE_LOG(LogTemp, Error, TEXT("Failed to create ViewModel of class: %s"), *ViewModelClass->GetName());
	return nullptr;
}

UCoreViewModel* UViewModelManagerSubsystem::GetViewModel(const TSubclassOf<UCoreViewModel> ViewModelClass)
{
	if (!IsValid(ViewModelClass))
	{
		return nullptr;
	}

	if (TObjectPtr<UCoreViewModel>* ExistingViewModel = ViewModels.Find(ViewModelClass))
	{
		if (IsValid(*ExistingViewModel))
		{
			return *ExistingViewModel;
		}
		else
		{
			// Clean up invalid entry
			ViewModels.Remove(ViewModelClass);
		}
	}

	return nullptr;
}

bool UViewModelManagerSubsystem::RemoveViewModel(const TSubclassOf<UCoreViewModel> ViewModelClass)
{
	if (!IsValid(ViewModelClass))
	{
		return false;
	}

	if (ViewModels.Contains(ViewModelClass))
	{
		ViewModels.Remove(ViewModelClass);
		UE_LOG(LogTemp, Log, TEXT("Removed ViewModel of class: %s"), *ViewModelClass->GetName());
		return true;
	}

	return false;
}
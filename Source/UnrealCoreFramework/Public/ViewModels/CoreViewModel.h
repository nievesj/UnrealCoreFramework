// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MVVMViewModelBase.h"

#include "CoreViewModel.generated.h"

class UViewModelManagerSubsystem;

/**
 * A minimal base class for MVVM view models providing only essential
 * lifecycle hooks that derived classes can override as needed.
 */
UCLASS(Blueprintable, Abstract, DisplayName = "Core base MVVM Viewmodel")
class UNREALCOREFRAMEWORK_API UCoreViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Initializes the ViewModel. Called once when the ViewModel is first created or activated. */
	virtual void Initialize();

	/** Refreshes the ViewModel's data and state. Can be called multiple times during the ViewModel's lifetime. */
	virtual void Refresh();

protected:
	/** Called during Initialize() to allow derived classes to perform custom initialization logic. */
	virtual void OnInitialized() {};

	/** Called during Refresh() to allow derived classes to perform custom refresh logic. */
	virtual void OnRefreshed() {};

	/** Reference to the ViewModel Manager Subsystem for centralized ViewModel operations. */
	UPROPERTY(Transient)
	TObjectPtr<UViewModelManagerSubsystem> ViewModelManagerSubsystem;
};
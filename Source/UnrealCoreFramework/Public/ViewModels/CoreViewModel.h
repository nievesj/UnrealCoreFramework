// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MVVMViewModelBase.h"

#include "CoreViewModel.generated.h"

class UViewModelManagerSubsystem;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreViewModel, Log, All);

/**
 * A minimal base class for MVVM view models providing only essential
 * lifecycle hooks that derived classes can override as needed.
 */
UCLASS(Blueprintable, Abstract, DisplayName = "Core base MVVM Viewmodel")
class UNREALCOREFRAMEWORK_API UCoreViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	/** Initializes the ViewModel - called once when first created or activated. */
	virtual void Initialize();

	/** Deinitializes the ViewModel - called once when deactivated or destroyed. */
	virtual void DeInitialize();

	/** Refreshes the ViewModel's data and state - can be called multiple times during lifetime. */
	UFUNCTION(BlueprintCallable, Category = "Core|ViewModel")
	virtual void Refresh();

	/** Blueprint event fired after the ViewModel has been refreshed. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Core|ViewModel")
	void OnRefreshed_BP();

	/** Template version for type-safe source object assignment with automatic initialization. */
	template <typename T>
	void SetSourceFromObject(const T* InSourceObject)
	{
		static_assert(std::is_base_of_v<UObject, T>, "T must inherit from UObject");
		Source = InSourceObject;

		Initialize();
	}

	/** Sets the source object that this ViewModel represents. */
	void SetSource(UObject* InSourceObject) { Source = InSourceObject; }

	/** Template version for type-safe source object retrieval. */
	template <typename T>
	T* GetSourceAs() const
	{
		static_assert(std::is_base_of_v<UObject, T>, "T must inherit from UObject");
		return Cast<T>(Source);
	}

	/** Returns the source object that this ViewModel represents. */
	UObject* GetSource() const { return Source; };

	/** Gets the player controller associated with this ViewModel. */
	UFUNCTION(BlueprintPure, Category = "Core|ViewModel")
	APlayerController* GetPlayerController() const;

	/** Gets the player character associated with this ViewModel. */
	UFUNCTION(BlueprintPure, Category = "Core|ViewModel")
	ACharacter* GetPlayerCharacter() const;

protected:
	/** Override to perform custom initialization logic during Initialize(). */
	virtual void OnInitialized() {};

	/** Override to perform custom refresh logic during Refresh(). */
	virtual void OnRefreshed() {};

	/** Reference to the ViewModel Manager Subsystem for centralized ViewModel operations. */
	UPROPERTY(Transient)
	TObjectPtr<UViewModelManagerSubsystem> ViewModelManagerSubsystem;

	/** The source object that this ViewModel represents and binds to. */
	UPROPERTY(Transient)
	TObjectPtr<UObject> Source;
};
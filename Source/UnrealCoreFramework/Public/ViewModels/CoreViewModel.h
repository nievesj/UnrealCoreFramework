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
		SetSource(InSourceObject);
	}

	/** Sets the source object that this ViewModel represents. */
	void SetSource(UObject* InSourceObject)
	{
		Source = InSourceObject;
		Initialize();
	}

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
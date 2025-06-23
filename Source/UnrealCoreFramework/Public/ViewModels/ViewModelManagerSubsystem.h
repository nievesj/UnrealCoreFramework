
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SubSystems/Base/CoreWorldSubsystem.h"

#include "ViewModelManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogViewModelManagerSubsystem, Log, All);

class UCoreViewModel;

/** A subsystem that manages ViewModel instances for creation, retrieval, and removal. */
UCLASS()
class UNREALCOREFRAMEWORK_API UViewModelManagerSubsystem : public UCoreWorldSubsystem
{
	GENERATED_BODY()

public:
	/** Creates or retrieves a ViewModel of the specified class for the given tracked object. */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|Management")
	UCoreViewModel* GetOrCreateViewModel(TSubclassOf<UCoreViewModel> ViewModelClass, const UObject* TrackedObject);

	/** Retrieves an existing ViewModel for the specified tracked object. */
	UCoreViewModel* GetModel(const UObject* TrackedObject);

	/** Template version for type-safe ViewModel creation and retrieval. */
	template <typename T>
	FORCEINLINE T* GetOrCreateViewModel(TSubclassOf<T> ViewModelClass, const UObject* TrackedObject)
	{
		static_assert(TIsDerivedFrom<T, UCoreViewModel>::IsDerived, "T must derive from UCoreViewModel");
		return Cast<T>(GetOrCreateViewModel(static_cast<TSubclassOf<UCoreViewModel>>(ViewModelClass), TrackedObject));
	}

	/** Removes the ViewModel associated with the specified tracked object. */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|Management")
	bool RemoveViewModel(const UObject* TrackedObject);

	/** Clears all ViewModels and properly deinitializes them. */
	void ClearModels();

protected:
	/** Map of object unique IDs to their associated ViewModel instances. */
	UPROPERTY(Transient)
	TMap<uint32, TObjectPtr<UCoreViewModel>> ViewModels;
};
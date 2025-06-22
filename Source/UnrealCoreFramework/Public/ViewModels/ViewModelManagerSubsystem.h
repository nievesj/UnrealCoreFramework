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
	/** Creates or retrieves a ViewModel of the specified class */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|Management")
	UCoreViewModel* CreateViewModel(TSubclassOf<UCoreViewModel> ViewModelClass);

	/** Template version for type-safe ViewModel creation */
	template <typename T>
	FORCEINLINE T* CreateViewModel(TSubclassOf<T> ViewModelClass)
	{
		static_assert(TIsDerivedFrom<T, UCoreViewModel>::IsDerived, "T must derive from UCoreViewModel");
		return Cast<T>(CreateViewModel(ViewModelClass));
	}

	/** Gets an existing ViewModel of the specified class */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|Management")
	UCoreViewModel* GetViewModel(TSubclassOf<UCoreViewModel> ViewModelClass);

	/** Template version for type-safe ViewModel retrieval */
	template <typename T>
	FORCEINLINE T* GetViewModel(TSubclassOf<T> ViewModelClass)
	{
		static_assert(TIsDerivedFrom<T, UCoreViewModel>::IsDerived, "T must derive from UCoreViewModel");
		return Cast<T>(GetViewModel(ViewModelClass));
	}

	/** Removes a ViewModel of the specified class */
	UFUNCTION(BlueprintCallable, Category = "ViewModel|Management")
	bool RemoveViewModel(TSubclassOf<UCoreViewModel> ViewModelClass);

protected:
	/** Map of ViewModel class types to their instances */
	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UCoreViewModel>> ViewModels;
};
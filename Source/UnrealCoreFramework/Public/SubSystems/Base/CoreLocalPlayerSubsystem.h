// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

#include "CoreLocalPlayerSubsystem.generated.h"

/**
 * ULocalPlayerSubsystem
 * Base class for auto instanced and initialized subsystem that share the lifetime of local players
 */

DECLARE_LOG_CATEGORY_EXTERN(LogLocalPlayerSubsystem, Log, All);

UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UCoreLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const
	{
		return true;
	}

	virtual void PreInitialize();

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void PostInitialize();

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	/** Overridden to check global network context */
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
};

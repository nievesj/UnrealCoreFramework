// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/EngineSubsystem.h"

#include "CoreEngineSubsystem.generated.h"

/**
 * UEngineSubsystem
 * Base class for auto instanced and initialized systems that share the lifetime of the Engine
 *
 * UEngineSubsystems are dynamic and will be initialized when the module is loaded if necessary.
 * This means that after StartupModule() is called on the module containing a subsystem,
 * the subsystem collection with instantiate and initialize the subsystem automatically.
 * If the subsystem collection is created post module load then the instances will be created at
 * collection initialization time.
 */

DECLARE_LOG_CATEGORY_EXTERN(LogEngineSubsystem, Log, All);

UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UCoreEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override
	{
		return true;
	}

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	/** Overridden to check global network context */
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/AudioEngineSubsystem.h"

#include "CoreAudioEngineSubSystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAudioSubsystem, Log, All);

/**
 * UAudioEngineSubsystem
 * Base class for auto instanced and initialized systems that share the lifetime of the audio device
 */

UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UCoreAudioEngineSubSystem : public UAudioEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const
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

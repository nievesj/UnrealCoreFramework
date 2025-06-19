// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "CoreTickableWorldSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTickableWorldSubsystem, Log, All);

/**
 * Base class for auto instanced and initialized systems that share the lifetime of a UWorld and are ticking along with it
 */
UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UCoreTickableWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PreInitialize();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void PostInitialize() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Deinitialize() override;
};

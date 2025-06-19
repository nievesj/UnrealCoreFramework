// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "CoreWorldSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreWorldSubsystem, Log, All);

/**
 * UWorldSubsystem
 * Base class for auto instanced and initialized systems that share the lifetime of a UWorld
 */

UCLASS(Abstract)
class UNREALCOREFRAMEWORK_API UCoreWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void PreInitialize();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Called once all UWorldSubsystems have been initialized */
	virtual void PostInitialize() override;

	/** Called when world is ready to start gameplay before the game mode transitions to the correct state and call BeginPlay on all actors */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	/** Called after world components (e.g. line batcher and all level components) have been updated */
	virtual void OnWorldComponentsUpdated(UWorld& World) override;

	/** Updates sub-system required streaming levels (called by world's UpdateStreamingState function) */
	virtual void UpdateStreamingState() override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	TFuture<void> SlowTaskAsync();
};

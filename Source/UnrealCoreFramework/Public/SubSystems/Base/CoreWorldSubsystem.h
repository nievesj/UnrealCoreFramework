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
};

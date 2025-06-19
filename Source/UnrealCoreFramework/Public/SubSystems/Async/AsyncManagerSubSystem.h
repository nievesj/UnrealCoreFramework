// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Ticker.h"
#include "SubSystems/Base/CoreGameInstanceSubsystem.h"

#include "AsyncManagerSubSystem.generated.h"

/**
 *
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UAsyncManagerSubSystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

	FTSTicker::FDelegateHandle TickHandler;

	bool Tick(float DeltaSeconds);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;
};

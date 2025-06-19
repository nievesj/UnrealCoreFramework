// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SubSystems/Base/CoreWorldSubsystem.h"

#include "SpawningSubsystem.generated.h"

/**
 *
 */
UCLASS()
class USpawningSubsystem : public UCoreWorldSubsystem
{
	GENERATED_BODY()

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};

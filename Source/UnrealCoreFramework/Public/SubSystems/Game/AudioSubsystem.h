// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SubSystems/Base/CoreAudioEngineSubSystem.h"

#include "AudioSubsystem.generated.h"

UCLASS()
class UNREALCOREFRAMEWORK_API UAudioSubsystem : public UCoreAudioEngineSubSystem
{
	GENERATED_BODY()

private:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
};

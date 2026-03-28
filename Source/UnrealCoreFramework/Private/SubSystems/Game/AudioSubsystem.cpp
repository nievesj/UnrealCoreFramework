// Fill out your copyright notice in the Description page of Project Settings.

#include "SubSystems/Game/AudioSubsystem.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Initializing Audio Subsystem"));
}

void UAudioSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitializing Audio Subsystem"))
}
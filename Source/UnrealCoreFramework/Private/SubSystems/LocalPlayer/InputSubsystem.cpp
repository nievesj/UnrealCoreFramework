// Fill out your copyright notice in the Description page of Project Settings.

#include "SubSystems/LocalPlayer/InputSubsystem.h"

void UInputSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("Initializing Input Subsystem"));
}

void UInputSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Deinitializing Input Subsystem"))
}
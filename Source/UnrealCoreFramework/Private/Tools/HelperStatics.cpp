// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/HelperStatics.h"

APlayerController* UHelperStatics::GetFirstPlayerController(const UObject* WorldContext)
{
	if (!IsValid(WorldContext))
	{
		return nullptr;
	}

	const UWorld* World = WorldContext->GetWorld();
	if (!IsValid(World))
	{
		return nullptr;
	}

	return World->GetFirstPlayerController();
}

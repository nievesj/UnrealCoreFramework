// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HelperStatics.generated.h"

/**
 * A utility class that provides static helper functions for various gameplay operations.
 * Inherits from UBlueprintFunctionLibrary to allow usage within Blueprints.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UHelperStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns the first player controller in the world.*/
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContext"))
	static APlayerController* GetFirstPlayerController(const UObject* WorldContext);
};

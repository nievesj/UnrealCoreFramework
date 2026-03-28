// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "CoreDataAsset.generated.h"

UCLASS(abstract, Meta = (LoadBehavior = "LazyOnDemand"))
class UNREALCOREFRAMEWORK_API UCoreDataAsset : public UDataAsset
{
	GENERATED_BODY()
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/DamageTypeDataAsset.h"

UDamageTypeBase::UDamageTypeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UDamageTypeDataAsset::PostLoad()
{
	Super::PostLoad();

	// Set default damage type if none specified
	if (!DamageTypeClass)
	{
		DamageTypeClass = UDamageType::StaticClass();
	}
}
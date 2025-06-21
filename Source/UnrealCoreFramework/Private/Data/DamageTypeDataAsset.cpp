// Fill out your copyright notice in the Description page of Project Settings.

#include "Data/DamageTypeDataAsset.h"

UCoreDamageType::UCoreDamageType(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FPrimaryAssetId UDamageTypeDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("DamageTypeDataAsset", GetFName());
}

void UDamageTypeDataAsset::PostLoad()
{
	Super::PostLoad();

	// Set default damage type if none specified
	if (!CoreDamageTypeClass)
	{
		CoreDamageTypeClass = UDamageType::StaticClass();
	}
}
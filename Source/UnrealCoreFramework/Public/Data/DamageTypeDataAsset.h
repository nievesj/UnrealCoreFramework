// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/DataAsset.h"

#include "DamageTypeDataAsset.generated.h"

/**
 * Base class for damage types.
 */
UCLASS(MinimalAPI, const, Blueprintable, BlueprintType)
class UCoreDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UCoreDamageType(const FObjectInitializer& ObjectInitializer);

	/** Multiplier to apply to base damage for this type. 1.0 means no change, higher means more damage. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (ClampMin = "0.0"))
	float BaseDamageMultiplier = 1.0f;

	/** True if this damage type applies a burning status effect. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	bool bCausesBurning = false;

	/** True if this damage type applies a poison effect. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	bool bCausesPoison = false;

	/** Duration in seconds for any status effect applied by this damage type. Zero disables effect. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effects")
	float StatusEffectDuration = 0.0f;
};

/**
 * Data asset holding configuration and visual/audio data
 * for a specific type of damage.
 *
 * Extend UPrimaryDataAsset to define custom damage types with extra metadata,
 * such as display options, effects, and modifiers.
 *
 * Used to drive visual feedback and gameplay interactions
 * when applying different kinds of damage.
 */
UCLASS(BlueprintType)
class UNREALCOREFRAMEWORK_API UDamageTypeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Class reference for the built-in Unreal damage type used for calculations. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Core")
	TSubclassOf<UCoreDamageType> CoreDamageTypeClass;

	/** Localized display name for UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText DisplayName;

	/** Extra description for UI or codex pages. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	FText Description;

	/** Icon to use for this damage type in UIs. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
	TSoftObjectPtr<UTexture2D> Icon;

	/** Particle system played when applying this damage type. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSoftObjectPtr<UParticleSystem> HitEffect;

	/** Audio cue played when this damage type is applied. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSoftObjectPtr<USoundBase> HitSound;

	/**
	 * Get the default object for the referenced damage type class.
	 *
	 * Returns the CDO for use in runtime systems.
	 */
	UFUNCTION(BlueprintPure, Category = "Damage Type")
	UCoreDamageType* GetDamageTypeCDO() const
	{
		return CoreDamageTypeClass ? CoreDamageTypeClass->GetDefaultObject<UCoreDamageType>() : nullptr;
	}

	/**
	 * Returns the unique PrimaryAssetId for this data asset. Used by Unreal's asset manager.
	 */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

protected:
	/**
	 * Called when the asset is finished loading. Override to run any custom post-load setup or validation.
	 */
	virtual void PostLoad() override;
};
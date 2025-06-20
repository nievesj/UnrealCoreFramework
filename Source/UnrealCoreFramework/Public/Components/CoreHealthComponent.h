#pragma once

#include "Components/ActorComponent.h"

#include "CoreHealthComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreHealthComponent, Log, All);

class UDamageTypeDataAsset;

/**
 * Delegate for health change events.
 *
 * @param HealthComp Reference to the health component.
 * @param Health     The updated health value.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, UCoreHealthComponent*, HealthComp, float, Health);

/**
 * Delegate for damage taken events.
 *
 * @param HealthComp Reference to the health component.
 * @param Damage     Amount of damage taken.
 * @param DamageType Type of damage received.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTakenSignature, UCoreHealthComponent*, HealthComp, float, Damage, const UDamageType*, DamageType);

/**
 * Delegate for death events.
 *
 * @param HealthComp Reference to the health component.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, UCoreHealthComponent*, HealthComp);

/**
 * Actor component that manages health, damage processing, regeneration and death.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALCOREFRAMEWORK_API UCoreHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor.
	 */
	UCoreHealthComponent();

	/** Maximum health value at start and after full healing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	/** Current health value, clamped between zero and MaxHealth. */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth;

	/** Rate at which health regenerates per second, if enabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.0"))
	float HealthRegenRate = 0.0f;

	/** Scalar applied to all incoming damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float DamageMultiplier = 1.0f;

	/** Delegate broadcast when health changes. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/** Delegate broadcast when damage is taken. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamageTakenSignature OnDamageTaken;

	/** Delegate broadcast when component health reaches zero. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	/**
	 * Set a new maximum health value and clamps current health if needed.
	 *
	 * @param NewMaxHealth The new maximum health value.
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void SetMaxHealth(float NewMaxHealth);

	/**
	 * Heal the actor by the specified amount.
	 *
	 * @param HealAmount Amount of health to restore.
	 * @return           True if healing was applied, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual bool Heal(float HealAmount);

	/**
	 * Get the percentage of current health relative to max health.
	 *
	 * @return Health percentage as a float between 0 and 1.
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	/**
	 * Check if the actor is considered dead.
	 *
	 * @return True if dead, false otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	/** Resistance multipliers mapped per damage type asset. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Resistance")
	TMap<TObjectPtr<UDamageTypeDataAsset>, float> DamageResistances;

protected:
	/**
	 * Called at game start.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called every frame.
	 *
	 * @param DeltaTime   Time since last frame.
	 * @param TickType    Tick type.
	 * @param ThisTickFunction Tick function context.
	 */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * Handle any damage event received by the owning actor.
	 *
	 * @param DamagedActor  Actor receiving damage.
	 * @param Damage        Amount of damage received.
	 * @param DamageType    Type of damage received.
	 * @param InstigatedBy  Controller responsible for causing damage.
	 * @param DamageCauser  Actor causing the damage.
	 */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/**
	 * Internal helper to set health value and trigger events.
	 *
	 * @param NewHealth Target health value.
	 */
	virtual void UpdateHealth(float NewHealth);

	/** True if death was already handled; prevents multiple death events. */
	bool bIsDead = false;
};
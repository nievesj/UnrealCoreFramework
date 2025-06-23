#pragma once

#include "Components/ActorComponent.h"

#include "CoreHealthComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCoreHealthComponent, Log, All);

class UDamageTypeDataAsset;
class UPlayerHealthViewModel; // Forward declaration

/** Delegate fired when health value changes. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, UCoreHealthComponent*, HealthComp, float, Health);

/** Delegate fired when damage is received. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTakenSignature, UCoreHealthComponent*, HealthComp, float, Damage, const UDamageType*, DamageType);

/** Delegate fired when health reaches zero. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, UCoreHealthComponent*, HealthComp);

/** Actor component that manages health, damage processing, regeneration and death. */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UNREALCOREFRAMEWORK_API UCoreHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** Default constructor. */
	UCoreHealthComponent();

	/** Delegate broadcast when health changes. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	/** Delegate broadcast when damage is taken. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamageTakenSignature OnDamageTaken;

	/** Delegate broadcast when component health reaches zero. */
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDeathSignature OnDeath;

	/** Gets the associated ViewModel for this health component. */
	UFUNCTION(BlueprintPure, Category = "Health|ViewModel")
	UPlayerHealthViewModel* GetHealthViewModel() const { return HealthViewModel; }

	/** Sets a new maximum health value and clamps current health if needed. */
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void SetMaxHealth(float NewMaxHealth);

	/** Returns current health as a percentage of maximum health (0.0 to 1.0). */
	float GetHealthPercent() const;

	/** Returns true if current health is zero or below. */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const;

	/** Restores health by the specified amount, returns true if healing was applied. */
	UFUNCTION(BlueprintPure, Category = "Health")
	bool Heal(float HealAmount);

	/** Returns the maximum health value. */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	/** Returns the current health value. */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	/** Returns the health regeneration rate per second. */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthRegenRate() const { return HealthRegenRate; }

	/** Returns the damage multiplier scalar applied to incoming damage. */
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetDamageMultiplier() const { return DamageMultiplier; }

protected:
	/** Called when the game starts or when spawned. */
	virtual void BeginPlay() override;

	/** Called when the component is destroyed. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called every frame to handle health regeneration. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Handles any damage event received by the owning actor. */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	/** Internal helper to set health value and trigger appropriate events. */
	virtual void UpdateHealth(float NewHealth);

	/** Maximum health value at start and after full healing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", Getter = GetMaxHealth, meta = (ClampMin = "0.0"), Setter = SetMaxHealth)
	float MaxHealth = 100.0f;

	/** Current health value, clamped between zero and MaxHealth. */
	UPROPERTY(BlueprintReadOnly, Category = "Health", Getter = GetCurrentHealth)
	float CurrentHealth;

	/** Rate at which health regenerates per second when enabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", Getter = GetHealthRegenRate, meta = (ClampMin = "0.0"))
	float HealthRegenRate = 0.0f;

	/** Scalar multiplier applied to all incoming damage. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", Getter = GetDamageMultiplier)
	float DamageMultiplier = 1.0f;

	/** Flag indicating if death was already handled to prevent multiple death events. */
	bool bIsDead = false;

	/** The ViewModel instance for this health component. */
	UPROPERTY(Transient)
	TObjectPtr<UPlayerHealthViewModel> HealthViewModel;
};
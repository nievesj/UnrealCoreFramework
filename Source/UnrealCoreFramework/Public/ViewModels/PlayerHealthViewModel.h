// PlayerHealthViewModel.h
#pragma once

#include "ViewModels/CoreViewModel.h"

#include "PlayerHealthViewModel.generated.h"

class UCoreHealthComponent;
class ADeityCharacter;

UCLASS(BlueprintType, Blueprintable)
class UNREALCOREFRAMEWORK_API UPlayerHealthViewModel : public UCoreViewModel
{
	GENERATED_BODY()

public:
	virtual void OnInitialized() override;
	virtual void DeInitialize() override;
	virtual void Refresh() override;

	// Initialize the ViewModel
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual void Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	UCoreHealthComponent* GetPlayerHealthComponent() const;

	// Properties for UI binding
	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float CurrentHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float MaxHealth = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float HealthPercentage = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float HealthRegenRate = 0.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	float DamageMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, FieldNotify)
	bool bIsDead = false;

protected:
	UFUNCTION()
	void HandleHealthChanged(UCoreHealthComponent* HealthComp, float Health);

	UFUNCTION()
	void HandleDamageTaken(UCoreHealthComponent* HealthComp, float Damage, const UDamageType* DamageType);

	UFUNCTION()
	void HandleDeath(UCoreHealthComponent* HealthComp);

private:
	// Keep reference to the health component we're listening to
	UPROPERTY(Transient)
	TObjectPtr<UCoreHealthComponent> HealthComponent;
};
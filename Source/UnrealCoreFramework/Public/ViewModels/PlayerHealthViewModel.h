// MIT License
//
// Copyright (c) 2026 José M. Nieves
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include "ViewModels/CoreViewModel.h"

#include "PlayerHealthViewModel.generated.h"

class UCoreHealthComponent;

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
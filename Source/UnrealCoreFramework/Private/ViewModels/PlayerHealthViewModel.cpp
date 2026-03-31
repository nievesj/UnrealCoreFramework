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
// PlayerHealthViewModel.cpp
#include "ViewModels/PlayerHealthViewModel.h"

#include "Components/CoreHealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UPlayerHealthViewModel::Initialize()
{
	UCoreHealthComponent* HealthComp = GetSourceAs<UCoreHealthComponent>();
	if (!IsValid(HealthComp))
	{
		UE_LOG(LogCoreViewModel, Error, TEXT("PlayerHealthViewModel: Could not cast health component"));
		return;
	}

	HealthComponent = HealthComp;

	// Subscribe to health events
	HealthComp->OnHealthChanged.AddDynamic(this, &UPlayerHealthViewModel::HandleHealthChanged);
	HealthComp->OnDamageTaken.AddDynamic(this, &UPlayerHealthViewModel::HandleDamageTaken);
	HealthComp->OnDeath.AddDynamic(this, &UPlayerHealthViewModel::HandleDeath);

	Super::Initialize();
}

void UPlayerHealthViewModel::OnInitialized()
{
	Super::OnInitialized();

	Refresh();
}
void UPlayerHealthViewModel::DeInitialize()
{
	// You should unsubscribe from events here
	if (IsValid(HealthComponent))
	{
		HealthComponent->OnHealthChanged.RemoveAll(this);
		HealthComponent->OnDamageTaken.RemoveAll(this);
		HealthComponent->OnDeath.RemoveAll(this);
	}

	HealthComponent = nullptr;
	Super::DeInitialize();
}

void UPlayerHealthViewModel::Refresh()
{
	if (!IsValid(HealthComponent))
	{
		UE_LOG(LogCoreViewModel, Error, TEXT("PlayerHealthViewModel: Health component is invalid"));
		return;
	}

	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, HealthComponent->GetCurrentHealth());
	UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, HealthComponent->GetMaxHealth());
	UE_MVVM_SET_PROPERTY_VALUE(HealthPercentage, HealthComponent->GetHealthPercent());
	UE_MVVM_SET_PROPERTY_VALUE(HealthRegenRate, HealthComponent->GetHealthRegenRate());
	UE_MVVM_SET_PROPERTY_VALUE(DamageMultiplier, HealthComponent->GetDamageMultiplier());
	UE_MVVM_SET_PROPERTY_VALUE(bIsDead, HealthComponent->IsDead());

	Super::Refresh();
}

UCoreHealthComponent* UPlayerHealthViewModel::GetPlayerHealthComponent() const
{
	return GetSourceAs<UCoreHealthComponent>();
}

void UPlayerHealthViewModel::HandleHealthChanged(UCoreHealthComponent* HealthComp, float Health)
{
	Refresh();
}

void UPlayerHealthViewModel::HandleDamageTaken(UCoreHealthComponent* HealthComp, float Damage, const UDamageType* DamageType)
{
	Refresh();
}

void UPlayerHealthViewModel::HandleDeath(UCoreHealthComponent* HealthComp)
{
	Refresh();
}
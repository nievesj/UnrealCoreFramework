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

#include "Components/CoreHealthComponent.h"

#include "GameFramework/Actor.h"
#include "Tools/SubsystemHelper.h"
#include "ViewModels/PlayerHealthViewModel.h"
#include "ViewModels/ViewModelManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogCoreHealthComponent);

UCoreHealthComponent::UCoreHealthComponent()
{
	CurrentHealth = 0;
	PrimaryComponentTick.bCanEverTick = true;
}

void UCoreHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!IsValid(Owner))
	{
		UE_LOG(LogCoreHealthComponent, Error, TEXT("CoreHealthComponent: Owner is invalid"));
		return;
	}

	UpdateHealth(MaxHealth);
	Owner->OnTakeAnyDamage.AddDynamic(this, &UCoreHealthComponent::HandleTakeAnyDamage);

	// Create and register the ViewModel
	UViewModelManagerSubsystem* ViewModelManager = USubsystemHelper::GetSubsystem<UViewModelManagerSubsystem>(this);
	if (IsValid(ViewModelManager))
	{
		HealthViewModel = ViewModelManager->GetOrCreateViewModel<UPlayerHealthViewModel>(this);
	}
}

void UCoreHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UViewModelManagerSubsystem* ViewModelManager = USubsystemHelper::GetSubsystem<UViewModelManagerSubsystem>(this);
	if (IsValid(ViewModelManager))
	{
		ViewModelManager->RemoveViewModel(this);
	}
	HealthViewModel = nullptr;
}

void UCoreHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Handle health regeneration if enabled
	if (HealthRegenRate > 0.0f && CurrentHealth > 0.0f && CurrentHealth < MaxHealth)
	{
		UpdateHealth(FMath::Min(MaxHealth, CurrentHealth + (HealthRegenRate * DeltaTime)));
	}
}

void UCoreHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	const float ActualDamage = FMath::Max(0.0f, Damage * DamageMultiplier);
	if (ActualDamage <= 0.0f)
	{
		return;
	}

	UpdateHealth(CurrentHealth - ActualDamage);
	OnDamageTaken.Broadcast(this, ActualDamage, DamageType);

	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(this);
	}
}

bool UCoreHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentHealth <= 0.0f || CurrentHealth >= MaxHealth)
	{
		return false;
	}

	UpdateHealth(FMath::Min(MaxHealth, CurrentHealth + HealAmount));
	return true;
}

void UCoreHealthComponent::UpdateHealth(float NewHealth)
{
	const float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

	if (!FMath::IsNearlyEqual(CurrentHealth, OldHealth))
	{
		OnHealthChanged.Broadcast(this, CurrentHealth);
	}
}

void UCoreHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	if (NewMaxHealth <= 0.0f)
	{
		return;
	}

	// Adjust current health proportionally
	const float HealthPercentage = MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
	MaxHealth = NewMaxHealth;
	UpdateHealth(MaxHealth * HealthPercentage);
}

float UCoreHealthComponent::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}

bool UCoreHealthComponent::IsDead() const
{
	return CurrentHealth <= 0.0f;
}
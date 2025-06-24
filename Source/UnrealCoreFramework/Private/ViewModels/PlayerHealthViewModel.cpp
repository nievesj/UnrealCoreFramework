// PlayerHealthViewModel.cpp
#include "ViewModels/PlayerHealthViewModel.h"

#include "Components/CoreHealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void UPlayerHealthViewModel::Initialize()
{
	UCoreHealthComponent* HealthComp = GetSourceAs<UCoreHealthComponent>();
	if (!IsValid(HealthComp))
		UE_LOG(LogCoreViewModel, Error, TEXT("PlayerHealthViewModel: Could not cast health component"));

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
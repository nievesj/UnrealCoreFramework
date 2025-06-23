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

	CurrentHealth = MaxHealth;
	Owner->OnTakeAnyDamage.AddDynamic(this, &UCoreHealthComponent::HandleTakeAnyDamage);

	// Create and register the ViewModel
	UViewModelManagerSubsystem* ViewModelManager = USubsystemHelper::GetSubsystem<UViewModelManagerSubsystem>(this);
	if (IsValid(ViewModelManager))
	{
		HealthViewModel = ViewModelManager->GetOrCreateViewModel<UPlayerHealthViewModel>(UPlayerHealthViewModel::StaticClass(), this);
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
		return;

	if (!IsValid(DamageType))
	{
		UE_LOG(LogCoreHealthComponent, Warning, TEXT("HandleTakeAnyDamage: DamageType is invalid. No damage applied."));
		return;
	}

	const float ActualDamage = Damage * DamageMultiplier;

	// Apply damage
	UpdateHealth(CurrentHealth - ActualDamage);

	OnDamageTaken.Broadcast(this, ActualDamage, DamageType);

	// Check for death
	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast(this);
	}
}

bool UCoreHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || CurrentHealth <= 0.0f || CurrentHealth >= MaxHealth)
		return false;

	UpdateHealth(FMath::Min(MaxHealth, CurrentHealth + HealAmount));
	return true;
}

void UCoreHealthComponent::UpdateHealth(float NewHealth)
{
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);

	if (CurrentHealth != OldHealth)
	{
		OnHealthChanged.Broadcast(this, CurrentHealth);
	}
}

void UCoreHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	if (NewMaxHealth <= 0.0f)
		return;

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
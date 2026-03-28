#include "CoreInteractionComponent.h"

#include "Engine/World.h"

UCoreInteractionComponent::UCoreInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCoreInteractionComponent::Interact_Implementation(AActor* Interactor)
{
	if (!Execute_CanInteract(this, Interactor))
	{
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		LastInteractionTime = World->GetTimeSeconds();
	}
	OnInteracted.Broadcast(Interactor);
}

FText UCoreInteractionComponent::GetInteractionPrompt_Implementation() const
{
	return PromptText;
}

bool UCoreInteractionComponent::CanInteract_Implementation(AActor* Interactor) const
{
	if (!bEnabled)
	{
		return false;
	}

	if (CooldownDuration > 0.0f)
	{
		const UWorld* World = GetWorld();
		if (!World)
		{
			return false;
		}
		const float CurrentTime = World->GetTimeSeconds();
		if (CurrentTime - LastInteractionTime < CooldownDuration)
		{
			return false;
		}
	}

	return true;
}


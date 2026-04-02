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

	if (InteractionRange > 0.0f && Interactor)
	{
		const AActor* Owner = GetOwner();
		if (Owner)
		{
			const float Distance = FVector::Dist(Owner->GetActorLocation(), Interactor->GetActorLocation());
			if (Distance > InteractionRange)
			{
				return false;
			}
		}
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
